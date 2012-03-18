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
#include "OperatingSystem.h"
#include "BossJob.h"
#include "Process.h"
#include "ExternalProcess.h"
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

int main(int argc, char** argv) {

  //               ==========================
  //                 Preliminary operations
  //               ==========================

   // LOGGING
#ifdef LOGL1
  cout << endl << "======> Job starts on " << OSUtils::getStrTime() << " on " << OSUtils::getHostName() << endl;
#endif

 // Job identifier
  if (argc < 2)
    return -1;
  string je = argv[0];
  string jepath = OSUtils::dirname(je);
  int id = atol(argv[1]);
  string strid = OSUtils::convert2string(id);
  string strpid = OSUtils::convert2string(OSUtils::getPid());

  // LOGGING
#ifdef LOGL1
  cout << endl << "======> Job ID is " << id << endl << endl;
#endif

  // Base directory. By default is current directory
  string basedir = ".";
  if ( argc > 2)
    basedir = argv[2];
//   if ( !OSUtils::dirExist( basedir ) ) {
//     cerr << "Directory " << basedir << " not found. Abort." << endl;
//     return -2;
//   }
#ifdef LOGL3
  cout << "Retrieving archive file from: " << basedir << endl;
#endif
  
  // Top working directory. By default is NONE
  string topworkdir = "NONE";
  // Working directory. By default is .
  string workdir = ".";
  if ( argc > 3)
    topworkdir = argv[3];
  if ( topworkdir != "NONE" ) {
    if ( topworkdir.substr(0,1) == "$" ) {
      string environ = OSUtils::getEnv(topworkdir.substr(1,topworkdir.size()-1));
      if ( environ != "" ) topworkdir = environ.c_str();
    }
    if ( !OSUtils::dirExist( topworkdir ) ) {
      cerr << "Directory " << topworkdir << " not found. Abort." << endl;
      return -3;
    }
    workdir = topworkdir + "/BossJob_" + strid;
    // be sure the directory is unnique
    if ( OSUtils::dirExist(workdir) ) {
      workdir+="_";
      int index=1;
      while ( OSUtils::dirExist(workdir+OSUtils::convert2string(index)) ) {
        index++;
      }
      workdir+=OSUtils::convert2string(index);
    }
    if ( OSUtils::makeDir(workdir) != 0 ) {
      cerr << "Cannot make working directory";
      return -4;
    }
    // change working directory
    OSUtils::changeDir(workdir);
  }
  // LOGGING
#ifdef LOGL3
  cout << "Working directory is: " << OSUtils::getCurrentDir() << endl;
#endif
  
  // Journal file to log updates
  string journalFile = string("BossJournal_")+strid+".txt";
  // Retrieve archive file
  string copycomm = "NONE";
  string archive = string("BossArchive_") + strid + ".tgz";
  if ( argc > 4)
    copycomm = argv[4];
  if ( copycomm != "NONE" ) {
    string command = copycomm+" "+basedir + "/" + archive+ " .";
    OSUtils::shell(command.c_str());
  }
  // Extract files from the archive
  if ( OSUtils::fileExist( archive ) ) {
    vector<string> dummy;
    if ( OSUtils::tar("xz",archive,dummy) ) {
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
  if ( OSUtils::fileExist( jobgenname ) ) {
    jobH = new BossJob();
    if ( jobH->setData(jobgenname,true) != 0 ) {
      delete jobH;
      return -7;
    }
    OSUtils::fileRemove(jobgenname);
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
      if ( OSUtils::fileExist( schemafile ) ) {
	ifstream sch(schemafile.c_str());
	if ( sch ) {
	  jobH->setSchema(*ti,sch);
	  sch.close();
	} else {
	  cerr << "Unable to read from schema file: " << schemafile << endl;
	  return -10;
	}
	OSUtils::fileRemove(schemafile);
	// check the local copy of the pre process script  
	if ( OSUtils::fileExist(string("./BossPre_")    +(*ti)+"_"+strid) )
	  do_pre.push_back(*ti);
	if ( OSUtils::fileExist(string("./BossRuntime_")+(*ti)+"_"+strid) )
	  do_runtime.push_back(*ti);
	if ( OSUtils::fileExist(string("./BossPost_")   +(*ti)+"_"+strid) )
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
  cout << "Executing host      : " << OSUtils::getHostName() << endl;
#endif

  // Set the job updator to be the through the journal file
  if ( OSUtils::touch(journalFile) != 0 ) {
    cerr << "Unable to create journal file " << journalFile << endl;
  }
  jobH->setUpdator(journalFile);
  OSUtils::append(journalFile,"[");
  // Start real time updator
  // (e.g. local, MySQL, R-GMAm etc...)
  // assume first it is in the same directory of jobExecutor 
  string updatorFile = jepath+"/dbUpdator"; 
  if ( !OSUtils::fileExist(updatorFile) )
    // if not try in the current directory
    updatorFile = "./dbUpdator";
  if ( !OSUtils::fileExist(updatorFile) )
    // last trial is to have it in the path
    updatorFile = OSUtils::which("dbUpdator");
  // be sure it is executable
  if ( OSUtils::fileExist(updatorFile) )
    if ( OSUtils::isMine(updatorFile) )
      OSUtils::fileChmod("777",updatorFile);
  OSUtils::Process* upd = 0;
  if ( updatorFile.size() == 0 ) {
    cerr << "Database updator file not found. Update has to be done manually" 
	 << endl;
  } else {
    cout << "Database updator file: " << updatorFile << endl;
    upd = new OSUtils::ExternalProcess(updatorFile,"/dev/null","","",strid);
    OSUtils::forkProcess(upd);
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

  filename = OSUtils::basename(filename);
  // change privileges
  OSUtils::fileChmod("744",filename);
  
  if(stdin_file  != "/dev/null")stdin_file  = OSUtils::basename(stdin_file);
  if(stdout_file != "/dev/null")stdout_file = OSUtils::basename(stdout_file);
  if(stderr_file != "/dev/null")stderr_file = OSUtils::basename(stderr_file);

#ifdef LOGL3
  cout << "Executable          : " << filename << endl;
  cout << "Std input file      : " << stdin_file << endl;
  cout << "Std output file     : " << stdout_file << endl;
  cout << "Std error file      : " << stderr_file << endl;
#endif
  
  // Set the execution variables
  jobH->setExeInfo(OSUtils::getHostName(),
		   OSUtils::getCurrentDir(),
		   OSUtils::getUserName(),
		   OSUtils::getTime());

  string tmpfile;

  // wait at most 5 seconds for processes linked to pipes and then kill
  int maxWait = 5;
  // maximum wait time for runtime process filters instead is user provided
  int maxRTWait = config->boss_max_retry()*config->boss_min_upd_int();

  //             ==============================
  //                   pre process stage
  //             ==============================
  // LOGGING
#ifdef LOGL2
  cout << endl << "======>> Preprocessing...       " << endl;
#endif  
  if ( stdin_file != "/dev/null" && OSUtils::fileExist(stdin_file) ) {
    for(ti=do_pre.begin(); ti<do_pre.end(); ti++) {
      tmpfile = string("./BossPre_")+(*ti)+"_"+strid;
      // create a tmp pipe
      string pre_pipe = tmpdir + string("/BossPrePipe-") + strpid;
      OSUtils::makeFIFO (pre_pipe, 0600);
      // create parser process
      OSUtils::Process* prep = new OSUtils::ExternalProcess(tmpfile,stdin_file,pre_pipe);
      OSUtils::forkProcess(prep);
      OSUtils::Process* labp = new BossLabellerProcess(id,(*ti),pre_pipe,journalFile);
      // read the pipe and update journal file
      labp->start();
      delete labp;
      OSUtils::waitProcessMaxTime(prep,maxWait);
#ifdef LOGL3
      cout << "Preprocess exited with code " << prep->retCode() << endl;
#endif
      delete prep;
      OSUtils::fileRemove(pre_pipe);
      OSUtils::fileRemove(tmpfile);
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
    OSUtils::makeFIFO (out_pipe, 0600); 
    string err_pipe = tmpdir + string("/BossErrPipe-") + strpid;
    OSUtils::makeFIFO (err_pipe, 0600);
    //  string filter_file = tmpdir + string("/BossFilterFile-") + strid;
    string filter_file = string("./BossFilterFile-") + strid;
    // now create the pipes for the splitters (one each jobtype)
    string tee_pipes = "";
    string pipes = "";
    vector<OSUtils::Process*> runtps;
    for(ti=do_runtime.begin(); ti<do_runtime.end(); ti++) {
      // check the local copy of the runtime process script 
      tmpfile = string("./BossRuntime_")    +(*ti)+"_"+strid;
      // create splitter tmp pipe
      string tee_pipe = tmpdir + string("/BossTeePipe-") + (*ti) + strpid;
      OSUtils::makeFIFO (tee_pipe, 0600); 
      // add pipe to tee argument list
      tee_pipes += tee_pipe + " ";
      pipes += tee_pipe + " ";
      // create the filter tmp pipe
      string filter_pipe = tmpdir + string("/BossFilterPipe-") + (*ti) + strpid;
      OSUtils::makeFIFO (filter_pipe, 0600); 
      pipes += filter_pipe + " ";
      // create parser process
      OSUtils::Process* runtp = new OSUtils::ExternalProcess(tmpfile,tee_pipe,filter_pipe);
      runtps.push_back(runtp);
      OSUtils::forkProcess(runtp);
      // create the "labeller" process
      OSUtils::Process* labp = new BossLabellerProcess(id,(*ti),filter_pipe,journalFile);
      runtps.push_back(labp);
      OSUtils::forkProcess(labp);
    }
    // create stdout splitter process
    OSUtils::Process* osplit = new OSUtils::ExternalProcess("/usr/bin/tee",out_pipe,stdout_file,"",tee_pipes);
    OSUtils::forkProcess(osplit);
    // create stderr splitter process
    OSUtils::Process* esplit = new OSUtils::ExternalProcess("/usr/bin/tee",err_pipe,stderr_file,"",tee_pipes);
    OSUtils::forkProcess(esplit);
    // create the user-job process
    OSUtils::Process* user = new OSUtils::ExternalProcess(filename,stdin_file,out_pipe,err_pipe,arguments);
    OSUtils::forkProcess(user);
    //
    // wait for the user job to finish
    //
    OSUtils::waitProcess(user);
    string ret_code = user->retCode();
    jobH->setRetCode(ret_code);
    delete user;
#ifdef LOGL3
    cout << "User job exited with code " << ret_code << endl;
#endif
    // wait also for the other processes to finish
    OSUtils::waitProcessMaxTime(osplit,maxRTWait);
#ifdef LOGL3
    cout << "STDOUT splitter exited with code " << osplit->retCode() << endl;
#endif
    delete osplit;
    OSUtils::waitProcessMaxTime(esplit,maxRTWait);
#ifdef LOGL3
    cout << "STDERR splitter exited with code " << esplit->retCode() << endl;
#endif
    delete esplit;
    for(vector<OSUtils::Process*>::const_iterator bpi=runtps.begin(); bpi<runtps.end(); bpi++ ) {
      OSUtils::waitProcessMaxTime(*bpi,maxRTWait);
#ifdef LOGL3
      cout << "Run-time process exited with code " << (*bpi)->retCode() << endl;
#endif
      delete *bpi;
    }
    OSUtils::fileRemove(out_pipe);
    OSUtils::fileRemove(err_pipe);
    OSUtils::fileRemove(pipes);
    OSUtils::fileRemove(filter_file);  
    OSUtils::fileRemove(tmpfile);  
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
    OSUtils::Process* user = new OSUtils::ExternalProcess(filename,stdin_file,stdout_file,stderr_file,arguments);
    OSUtils::forkProcess(user);
    // wait for the user job to finish
    OSUtils::waitProcess(user);
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
  if ( ( stdout_file != "/dev/null" && OSUtils::fileExist(stdout_file) ) ||
       ( stderr_file != "/dev/null" && OSUtils::fileExist(stderr_file) ) ) {
    for(ti=do_post.begin(); ti<do_post.end(); ti++) {
      // check the local copy of the pre process script  
      tmpfile = string("./BossPost_")+(*ti)+"_"+strid;
      if ( stdout_file != "/dev/null" && OSUtils::fileExist(stdout_file) ) {
	// create a tmp pipe
	string post_pipe = tmpdir + string("/BossPostOutPipe-") + strpid;
	OSUtils::makeFIFO (post_pipe, 0600); 
	// create parser process for stdout
	OSUtils::Process* postp = new OSUtils::ExternalProcess(tmpfile,stdout_file,post_pipe);
	OSUtils::forkProcess(postp);
        OSUtils::Process* labp = new BossLabellerProcess(id,(*ti),post_pipe,journalFile);
	// read the pipe and update journal file
	labp->start();
	delete labp;
	OSUtils::waitProcessMaxTime(postp,maxWait);
#ifdef LOGL3
	cout << "STDOUT post process exited with code " << postp->retCode() << endl;
#endif
	delete postp;
	// remove tmp pipe
	OSUtils::fileRemove(post_pipe);
	cout << "Removed " << post_pipe << endl;
      }
      if ( stderr_file != "/dev/null" && OSUtils::fileExist(stderr_file) ) {
	// create a tmp pipe
	string post_pipe = tmpdir + string("/BossPostErrPipe-") + strpid;
	OSUtils::makeFIFO (post_pipe, 0600); 
	// create parser process for stderr
	OSUtils::Process* errp = new OSUtils::ExternalProcess(tmpfile,stderr_file,post_pipe);
	OSUtils::forkProcess(errp);
	OSUtils::Process* labp1 = new BossLabellerProcess(id,(*ti),post_pipe,journalFile);
	// read the pipe and update journal file
	labp1->start();
	delete labp1;
	OSUtils::waitProcessMaxTime(errp,maxWait);
#ifdef LOGL3
	cout << "STDERR post process exited with code " << errp->retCode() << endl;
#endif
	delete errp;
	// remove tmp pipe
	OSUtils::fileRemove(post_pipe);
	cout << "Removed " << post_pipe << endl;
      }
      // remove post-process filter
      OSUtils::fileRemove(tmpfile);    
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
  // Set stop and stat time
  jobH->setStopTime(OSUtils::getTime());
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
  cout << "\n======> jobExecutor " << jobH->getId() << " finished at " << OSUtils::getStrTime() << endl;
#endif
  
  // Terminate the database updator, if any
  OSUtils::append(journalFile,"]");
  if (upd) {
    OSUtils::waitProcessMaxTime(upd,maxRTWait);
    delete upd;
#ifdef LOGL3
     cout << "DB Updator process terminated" << endl;
#endif
  }

  // Pack output sandbox
  bool outtarcopied = false;
  std::vector<std::string> outf = jobH->getOutFiles();
  outf.push_back(journalFile);
  std::string outtar = std::string("BossOutArchive_")+strid+".tgz";
  int tarerr = OSUtils::tar("cz",outtar,outf);
  if (tarerr)
    std::cerr << "Error creating output archive" << std::endl;
  else {
    if ( copycomm != "NONE" ) {
      string command = copycomm+" "+outtar + " " + basedir;
      outtarcopied = OSUtils::shell(command.c_str()) == 0;
    }
  }

  // clean-up
  std::string cleanfiles = "BossConfig.clad ";
  cleanfiles += archive + " ";
  std::vector<std::string> inf = jobH->getInFiles();
  std::vector<std::string>::const_iterator itf;
  for (itf=inf.begin();itf!=inf.end(); ++itf) {
    if ( OSUtils::fileExist(*itf) )
      cleanfiles += OSUtils::basename(*itf) + " ";
  }
  if (!tarerr) {
    for (itf=outf.begin();itf!=outf.end(); ++itf) {
      if ( OSUtils::fileExist(*itf) )
	cleanfiles += (*itf) + " ";
    }
    if (outtarcopied)
      cleanfiles += outtar;
  } 
  OSUtils::fileRemove(cleanfiles);

  // Delete the Job handle
  delete jobH;
  
  return 0;
}
