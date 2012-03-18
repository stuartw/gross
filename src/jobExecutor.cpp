// /////////////////////////////////////////////////////////////////////
// Program: jobExecutor
// Version: 3.0
// File:    jobExecutor.cpp
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    20/09/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <string>
#include <sys/times.h> 

using namespace std;

#include "BossConfiguration.h"
#include "BossOperatingSystem.h"
#include "BossJob.h"
#include "BossExternalProcess.h"
#include "BossLabellerProcess.h"

string redirectDir(string base_dir, string dir) {
  string outdir;
  if ( dir.size() > 0 )
    if ( dir[0] == '/' )
      outdir = dir; // absolute path
    else
      outdir = base_dir + string("/") + dir; // relative path
  return outdir;
}
//
//
// arguments to jobExecutor are:
// argv[0] jobExecutor
// argv[1] BOSS job ID
// argv[2] base directory (where to find the BOSS archive) 
// argv[3] top working directory 
//         <TOP_WORK_DIR>/<JobID> is created if different from "NONE" 
// argv[4] localIO flag

int main(int argc, char** argv) {

  //               ==========================
  //                 Preliminary operations
  //               ==========================

  BossOperatingSystem* sys=BossOperatingSystem::instance();

   // LOGGING
#ifdef LOGL1
  cout << endl << "======> Job starts on " << sys->getStrTime() << " on " << sys->getHostName() << endl;
#endif

 // Job identifier
  if (argc < 2)
    return -1;
  string je = argv[0];
  string jepath = sys->dirname(je);
  int id = atol(argv[1]);
  string strid = sys->convert2string(id);
  string strpid = sys->convert2string(sys->getPid());

  // LOGGING
#ifdef LOGL1
  cout << endl << "======> Job ID is " << id << endl << endl;
#endif

  // Base directory. By default is current directory
  string basedir = ".";
  if ( argc > 2)
    basedir = argv[2];
  if ( !sys->dirExist( basedir ) ) {
    cerr << "Directory " << basedir << " not found. Abort." << endl;
    return -2;
  }
#ifdef LOGL3
  cout << "Base directory is: " << basedir << endl;
#endif
  
  // Top working directory. By default is NONE
  string topworkdir = "NONE";
  // Working directory. By default is .
  string workdir = ".";
  if ( argc > 3)
    topworkdir = argv[3];
  if ( topworkdir != "NONE" ) {
    if ( topworkdir.substr(0,1) == "$" ) {
      string environ = sys->getEnv(topworkdir.substr(1,topworkdir.size()-1));
      if ( environ != "" ) topworkdir = environ.c_str();
    }
    if ( !sys->dirExist( topworkdir ) ) {
      cerr << "Directory " << topworkdir << " not found. Abort." << endl;
      return -3;
    }
    workdir = topworkdir + "/BossJob_" + strid;
    // be sure the directory is unnique
    if ( sys->dirExist(workdir) ) {
      workdir+="_";
      int index=1;
      while ( sys->dirExist(workdir+sys->convert2string(index)) ) {
        index++;
      }
      workdir+=sys->convert2string(index);
    }
    if ( sys->makeDir(workdir) != 0 ) {
      cerr << "Cannot make working directory";
      return -4;
    }
    // change working directory
    sys->changeDir(workdir);
  }
  // LOGGING
#ifdef LOGL3
  cout << "Working directory is: " << sys->getCurrentDir() << endl;
#endif
  
  // Check if standard I/O has to be local (i.e. current directory)
  bool localIO = false;
  if ( argc > 4)
    localIO = string(argv[4]) == string("localIO");
  
  // Journal file to log updates
  string journalFile = string("BossJournal_")+strid+".txt";

  // Extract files from the archive
  string archive = basedir + "/" + string("BossArchive_") + strid + ".tgz";
  if ( sys->fileExist( archive ) ) {
    vector<string> dummy;
    if ( sys->tar("xz",archive,dummy) ) {
      cerr << "Error extracting files from archive" << endl;
      return -5;
    }
  } else {
    cerr << "Archive file doesn't exist" << endl;
    return -6;
  }
#ifdef LOGL3
  cout << "Files extracted from archive:" << archive << endl;
#endif

  // Read Boss database configuration
  BossConfiguration* config=BossConfiguration::instance();

  // Other parameters
  string tmpdir = config->boss_tmp_dir();
  
  // Create and fill the job instance
  BossJob* jobH = 0;
  string jobgenname = "./" + string("BossGeneralInfo_") + strid;
  if ( sys->fileExist( jobgenname ) ) {
    jobH = new BossJob();
    if ( jobH->setData(jobgenname,true) != 0 ) {
      delete jobH;
      return -7;
    }
    sys->fileRemove(jobgenname);
    if ( id != jobH->getId() ) {
      cerr << "JobId in job info file does not match requested ID. Abort." 
	   << endl;
      return -8;
    }
  } else {
    cerr << "Unable to find job general info file: " << jobgenname << endl;
    return -9;
  }
  // Specific part
  vector<string> do_pre;
  vector<string> do_runtime;
  vector<string> do_post;
  vector<string>::const_iterator ti;
  vector<string> types = jobH->getJobTypes();
  for(ti=types.begin(); ti<types.end(); ti++) {
    if ( *ti != "stdjob" ) {
      string schemafile = "./"+string("BossSchema_")+(*ti)+"_"+strid;
      if ( sys->fileExist( schemafile ) ) {
	ifstream sch(schemafile.c_str());
	if ( sch ) {
	  jobH->setSchema(*ti,sch);
	  sch.close();
	} else {
	  cerr << "Unable to read from schema file: " << schemafile << endl;
	  return -10;
	}
	sys->fileRemove(schemafile);
	// check the local copy of the pre process script  
	if ( sys->fileExist(string("./BossPre_")    +(*ti)+"_"+strid) )
	  do_pre.push_back(*ti);
	if ( sys->fileExist(string("./BossRuntime_")+(*ti)+"_"+strid) )
	  do_runtime.push_back(*ti);
	if ( sys->fileExist(string("./BossPost_")   +(*ti)+"_"+strid) )
	  do_post.push_back(*ti);
      } else {
	cerr << "Unable to find schema file: " << schemafile << endl;
	return -11;
      }
    }
  }

  // Check job handle
  if(jobH==0) {
    cerr << "Cannot access job " << id << ". Aborting" << endl;
    return -12;
  }

  // LOGGING
#ifdef LOGL2
  cout << "Executing host      : " << sys->getHostName() << endl;
#endif

  // Set the job updator to be the through the journal file
  if ( sys->touch(journalFile) != 0 ) {
    cerr << "Unable to create journal file " << journalFile << endl;
  }
  jobH->setUpdator(journalFile);
  sys->append(journalFile,"[");
  // Start real time updator
  // (e.g. local, MySQL, R-GMAm etc...)
  // assume first it is in the same directory of jobExecutor 
  string updatorFile = jepath+"/dbUpdator"; 
  if ( !sys->fileExist(updatorFile) )
    // if not try in the current directory
    updatorFile = "./dbUpdator";
  if ( !sys->fileExist(updatorFile) )
    // last trial is to have it in the path
    updatorFile = sys->which("dbUpdator");
  // be sure it is executable
  if ( sys->fileExist(updatorFile) )
    if ( sys->isMine(updatorFile) )
      sys->fileChmod("777",updatorFile);
  BossProcess* upd = 0;
  if ( updatorFile.size() == 0 ) {
    cerr << "Database updator file not found. Update has to be done manually" 
	 << endl;
  } else {
    cout << "Database updator file: " << updatorFile << endl;
    upd = new BossExternalProcess(updatorFile,"/dev/null","","",strid);
    sys->forkProcess(upd);
  }

  // Get job parameters
  string executable=jobH->getExecutable();
  string stdInput=jobH->getStdin();
  string stdOutput=jobH->getStdout();
  string stdError=jobH->getStderr();
  string arguments=jobH->getArguments();
  string old_dir=jobH->getSubPath();
  string log=jobH->getLog();
  // adjust file with absolute path
  string filename = redirectDir(old_dir, executable);
  string stdin_file = redirectDir(old_dir, stdInput);
  string stdout_file = redirectDir(old_dir, stdOutput);
  string stderr_file = redirectDir(old_dir, stdError);

  if ( localIO ) {
#ifdef LOGL1
    cout << "Using Local I/O option for executable and stdin/out/err" << endl;
#endif
    // this is needed when running jobs on the grid
    filename = sys->basename(filename);
    // change privileges
    sys->fileChmod("744",filename);
    
    if(stdin_file  != "/dev/null")stdin_file  = sys->basename(stdin_file);
    if(stdout_file != "/dev/null")stdout_file = sys->basename(stdout_file);
    if(stderr_file != "/dev/null")stderr_file = sys->basename(stderr_file);
    if(log         != "/dev/null")log         = sys->basename(log);
  }

#ifdef LOGL3
  cout << "Executable          : " << filename << endl;
  cout << "Std input file      : " << stdin_file << endl;
  cout << "Std output file     : " << stdout_file << endl;
  cout << "Std error file      : " << stderr_file << endl;
#endif
  
  // Set the execution variables
  jobH->setExeInfo(sys->getHostName(),
		   sys->getCurrentDir(),
		   sys->getUserName(),
		   sys->getTime());

  string tmpfile;

  // wait at most 5 seconds for processes linked to pipes and then kill
  int maxWait = 5;
  // maximum wait time for runtime process filters instead is user provided
  int maxRTWait = config->boss_max_retry()*config->boss_upd_interval();

  //             ==============================
  //                   pre process stage
  //             ==============================
  // LOGGING
#ifdef LOGL2
  cout << endl << "======>> Preprocessing...       " << endl;
#endif  
  if ( stdin_file != "/dev/null" && sys->fileExist(stdin_file) ) {
    for(ti=do_pre.begin(); ti<do_pre.end(); ti++) {
      tmpfile = string("./BossPre_")+(*ti)+"_"+strid;
      // create a tmp pipe
      string pre_pipe = tmpdir + string("/BossPrePipe-") + strpid;
      sys->makeFIFO (pre_pipe, 0600);
      // create parser process
      BossProcess* prep = new BossExternalProcess(tmpfile,stdin_file,pre_pipe);
      sys->forkProcess(prep);
      BossProcess* labp = new BossLabellerProcess(id,(*ti),pre_pipe,journalFile);
      // read the pipe and update journal file
      labp->start();
      delete labp;
      sys->waitProcessMaxTime(prep,maxWait);
#ifdef LOGL3
      cout << "Preprocess exited with code " << prep->retCode() << endl;
#endif
      delete prep;
      sys->fileRemove(pre_pipe);
      sys->fileRemove(tmpfile);
      // LOGGING
#ifdef LOGL3
      cout << "Removed " << pre_pipe << endl;
      cout << "Removed " << tmpfile << endl;
#endif  
    }
  }
    // LOGGING
#ifdef LOGL2
  cout << "======>> ...preprocess stage ended  " << endl;
#endif
    
  //             ==============================
  //                  runtime process stage
  //             ==============================
  // LOGGING
#ifdef LOGL2
  cout << endl << "======>> runtime processing...    " << endl;
#endif
  // Check if runtime monitoring has to be done
  if ( do_runtime.size()>0 ) {
    // put runtimeprocess in backgroung
    // fork this process:
    //                    child execute real job 
    //                    father read job output and update db in real time
    
    // create STDOUT and STDERR tmp pipes
    string out_pipe = tmpdir + string("/BossOutPipe-") + strpid;
    sys->makeFIFO (out_pipe, 0600); 
    string err_pipe = tmpdir + string("/BossErrPipe-") + strpid;
    sys->makeFIFO (err_pipe, 0600);
    //  string filter_file = tmpdir + string("/BossFilterFile-") + strid;
    string filter_file = string("./BossFilterFile-") + strid;
    // now create the pipes for the splitters (one each jobtype)
    string tee_pipes = "";
    string pipes = "";
    vector<BossProcess*> runtps;
    for(ti=do_runtime.begin(); ti<do_runtime.end(); ti++) {
      // check the local copy of the runtime process script 
      tmpfile = string("./BossRuntime_")    +(*ti)+"_"+strid;
      // create splitter tmp pipe
      string tee_pipe = tmpdir + string("/BossTeePipe-") + (*ti) + strpid;
      sys->makeFIFO (tee_pipe, 0600); 
      // add pipe to tee argument list
      tee_pipes += tee_pipe + " ";
      pipes += tee_pipe + " ";
      // create the filter tmp pipe
      string filter_pipe = tmpdir + string("/BossFilterPipe-") + (*ti) + strpid;
      sys->makeFIFO (filter_pipe, 0600); 
      pipes += filter_pipe + " ";
      // create parser process
      BossProcess* runtp = new BossExternalProcess(tmpfile,tee_pipe,filter_pipe);
      runtps.push_back(runtp);
      sys->forkProcess(runtp);
      // create the "labeller" process
      BossProcess* labp = new BossLabellerProcess(id,(*ti),filter_pipe,journalFile);
      runtps.push_back(labp);
      sys->forkProcess(labp);
    }
    // create stdout splitter process
    BossProcess* osplit = new BossExternalProcess("/usr/bin/tee",out_pipe,stdout_file,"",tee_pipes);
    sys->forkProcess(osplit);
    // create stderr splitter process
    BossProcess* esplit = new BossExternalProcess("/usr/bin/tee",err_pipe,stderr_file,"",tee_pipes);
    sys->forkProcess(esplit);
    // create the user-job process
    BossProcess* user = new BossExternalProcess(filename,stdin_file,out_pipe,err_pipe,arguments);
    sys->forkProcess(user);
    //
    // wait for the user job to finish
    //
    sys->waitProcess(user);
    string ret_code = user->retCode();
    jobH->setRetCode(ret_code);
    delete user;
#ifdef LOGL3
    cout << "User job exited with code " << ret_code << endl;
#endif
    // wait also for the other processes to finish
    sys->waitProcessMaxTime(osplit,maxRTWait);
#ifdef LOGL3
    cout << "STDOUT splitter exited with code " << osplit->retCode() << endl;
#endif
    delete osplit;
    sys->waitProcessMaxTime(esplit,maxRTWait);
#ifdef LOGL3
    cout << "STDERR splitter exited with code " << esplit->retCode() << endl;
#endif
    delete esplit;
    for(vector<BossProcess*>::const_iterator bpi=runtps.begin(); bpi<runtps.end(); bpi++ ) {
      sys->waitProcessMaxTime(*bpi,maxRTWait);
#ifdef LOGL3
      cout << "Run-time process exited with code " << (*bpi)->retCode() << endl;
#endif
      delete *bpi;
    }
    sys->fileRemove(out_pipe);
    sys->fileRemove(err_pipe);
    sys->fileRemove(pipes);
    sys->fileRemove(filter_file);  
    sys->fileRemove(tmpfile);  
    // LOGGING
#ifdef LOGL3
    cout << "Removed " << out_pipe << endl;
    cout << "Removed " << err_pipe << endl;
    cout << "Removed " << pipes << endl;
    cout << "Removed " << filter_file << endl;
    cout << "Removed " << tmpfile << endl;
#endif     
  } else {
    cout << "======>> No monitoring requested. Start execution"  << endl << endl;
    // No monitoring needed. Just fork the user executable
    BossProcess* user = new BossExternalProcess(filename,stdin_file,stdout_file,stderr_file,arguments);
    sys->forkProcess(user);
    // wait for the user job to finish
    sys->waitProcess(user);
    string ret_code = user->retCode();
    jobH->setRetCode(ret_code);
    delete user;
#ifdef LOGL3
    cout << "User job exited with code " << ret_code << endl;
#endif
    cout << "======>> Execution finished"  << endl << endl;
  }
  // LOGGING
#ifdef LOGL2
  cout << "======>> ...runtime stage ended    "  << endl;
#endif  
  
  //             ==============================
  //                   post process stage
  //             ==============================
  // LOGGING
#ifdef LOGL2
  cout << endl << "======>> Postprocessing...       " << endl;
#endif  
  if ( ( stdout_file != "/dev/null" && sys->fileExist(stdout_file) ) ||
       ( stderr_file != "/dev/null" && sys->fileExist(stderr_file) ) ) {
    for(ti=do_post.begin(); ti<do_post.end(); ti++) {
      // check the local copy of the pre process script  
      tmpfile = string("./BossPost_")+(*ti)+"_"+strid;
      if ( stdout_file != "/dev/null" && sys->fileExist(stdout_file) ) {
	// create a tmp pipe
	string post_pipe = tmpdir + string("/BossPostOutPipe-") + strpid;
	sys->makeFIFO (post_pipe, 0600); 
	// create parser process for stdout
	BossProcess* postp = new BossExternalProcess(tmpfile,stdout_file,post_pipe);
	sys->forkProcess(postp);
        BossProcess* labp = new BossLabellerProcess(id,(*ti),post_pipe,journalFile);
	// read the pipe and update journal file
	labp->start();
	delete labp;
	sys->waitProcessMaxTime(postp,maxWait);
#ifdef LOGL3
	cout << "STDOUT post process exited with code " << postp->retCode() << endl;
#endif
	delete postp;
	// remove tmp pipe
	sys->fileRemove(post_pipe);
	cout << "Removed " << post_pipe << endl;
      }
      if ( stderr_file != "/dev/null" && sys->fileExist(stderr_file) ) {
	// create a tmp pipe
	string post_pipe = tmpdir + string("/BossPostErrPipe-") + strpid;
	sys->makeFIFO (post_pipe, 0600); 
	// create parser process for stderr
	BossProcess* errp = new BossExternalProcess(tmpfile,stderr_file,post_pipe);
	sys->forkProcess(errp);
	BossProcess* labp1 = new BossLabellerProcess(id,(*ti),post_pipe,journalFile);
	// read the pipe and update journal file
	labp1->start();
	delete labp1;
	sys->waitProcessMaxTime(errp,maxWait);
#ifdef LOGL3
	cout << "STDERR post process exited with code " << errp->retCode() << endl;
#endif
	delete errp;
	// remove tmp pipe
	sys->fileRemove(post_pipe);
	cout << "Removed " << post_pipe << endl;
      }
      // remove post-process filter
      sys->fileRemove(tmpfile);    
      // LOGGING
#ifdef LOGL3
      cout << "Removed " << tmpfile << endl;
#endif
    }
  }
  cout << "======>> ...postprocess stage ended    "  << endl << endl;
  //               =====================
  //                 Final operations
  //               =====================
  // clean-up
  sys->fileRemove(archive);
  sys->fileRemove("BossConfig.clad");
  // Set stop and stat time
  jobH->setStopTime(sys->getTime());
  struct tms time_stat;
  times(&time_stat);
  long clkxtck = sysconf(_SC_CLK_TCK);
  float jut = (float)time_stat.tms_cutime/clkxtck;
  float jst = (float)time_stat.tms_cstime/clkxtck;
#ifdef LOGL2 
  float but = (float)time_stat.tms_utime/clkxtck;  
  float bst = (float)time_stat.tms_stime/clkxtck; 
#endif 
  char buf_cvt[100];
  jobH->setStatTime(string(gcvt(jut, 2, buf_cvt))+"s user "+
		    string(gcvt(jst, 2, buf_cvt))+"s sys");
  
  // LOGGING
#ifdef LOGL2 
  cout << "Time statistics of BOSS system" << endl;
  cout << "Job user time             : " << jut 
       << " seconds" << endl;
  cout << "Job system time           : " << jst
       <<" seconds" <<  endl;
  cout << "BOSS overload user time   : " << but
       << " seconds" << endl;
  cout << "BOSS overload system time : " << bst
       << " seconds" << endl;
  cout << "Elapsed time (included pre and post process): " 
       << (float)jobH->getStopTime() - jobH->getStartTime() 
       << " seconds" << endl;
  cout << "Total elapsed time from submission to end   : "
       << (float)jobH->getStopTime() - jobH->getStartTime() 
       << " seconds" << endl;
#endif
#ifdef LOGL1
  cout << "\n======> jobExecutor " << jobH->getId() << " finished at " << sys->getStrTime() << endl;
#endif
  
  // Delete the Job handle
  delete jobH;
  
  
  // Terminate the database updator, if any
  sys->append(journalFile,"]");
  if (upd) {
    sys->waitProcessMaxTime(upd,maxRTWait);
    delete upd;
#ifdef LOGL3
     cout << "DB Updator process terminated" << endl;
#endif
  }
  return 0;
}
