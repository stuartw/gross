// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossRecoverJob.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <string>
#include <iostream>

#include "BossRecoverJob.h"

#include "BossConfiguration.h"
#include "BossDatabase.h"
#include "BossScheduler.h"
#include "BossOperatingSystem.h"
#include "BossJob.h"
#include "BossExternalProcess.h"
#include "BossLabellerProcess.h"

using namespace std;

BossRecoverJob::BossRecoverJob() : BossCommand() {
  opt_["-jobid"] = ""; 
  opt_["-noprompt"] = "FALSE"; 
  opt_["-localIO"] = "FALSE";
  opt_["-journal"] = "";
}

BossRecoverJob::~BossRecoverJob() {}

void BossRecoverJob::printUsage() const
{
  cout << "Usage:" << endl
       << "boss recoverJob " << endl;
  cout << "           -jobid <job id> " << endl
       << "           -noprompt " << endl
       << "           -localIO " << endl
       << "           -journal <journal file> " << endl
       << endl;
}

int BossRecoverJob::execute() {

  //  for (Options_const_iterator i=opt_.begin();i!=opt_.end();i++)
  //    cout << i->first << "=" << i->second << endl;
  
  BossOperatingSystem* sys=BossOperatingSystem::instance();
  BossDatabase db("super");
  int my_pid = getpid();
  BossJob* jobH;  

  BossConfiguration* config=BossConfiguration::instance();
  string tmpdir = config->boss_tmp_dir();

  if ( opt_["-jobid"] == "" ) { 
    return -1;
  }
  int id = atol((opt_["-jobid"]).c_str());

  // check if the job exists
  jobH = db.findJob(id);
  if ( !jobH ) {
    cout << "JobID " << id << " not found" << endl;
    return -3;
  }
  // check if the job is not running 
  if ( checkRunning(jobH, &db) ) {
    cerr << "the job is scheduled or running, please wait or kill it" << endl;
    return -2;
  }

  if ( opt_["-noprompt"] == "FALSE" && !prompt() ) {
    return -4;
  }

  // Recover from journal
  string journal = opt_["-journal"];
  if ( journal != "" ) {
    if (!sys->fileExist(journal)) {
      cerr << "Journal file not found: " << journal << endl;
      return -5;
    }
  } else {    
    // Try to re-run the post-process script on STDOUT/ERR
    journal = string("BossJournal_Recovered_")+opt_["-jobid"]+".txt";
    //try first with a local journal:
    if ( sys->touch(journal) != 0 ) {
      // in case of error try with a journal in boss temporary dir:
      journal = tmpdir + "/" + journal;
      if ( sys->touch(journal) != 0 ) {
	cerr << "Unable to create a local journal file " << journal << endl;
	return -6;
      }
    }
    sys->append(journal,"[");

    // Get job parameters
    string old_dir=jobH->getSubPath();
    string stdOutput=jobH->getStdout();
    string stdError=jobH->getStderr();
    // adjust file with absolute path
    string stdout_file = redirectDir(old_dir, stdOutput);
    string stderr_file = redirectDir(old_dir, stdError);
    if ( opt_["-localIO"] == "TRUE" ) {
      if(stdout_file != "/dev/null")stdout_file = sys->basename(stdout_file);
      if(stderr_file != "/dev/null")stderr_file = sys->basename(stderr_file);
    }
    // Get the post-process script
    vector<string> types = jobH->getJobTypes();
    for(vector<string>::const_iterator ti=types.begin(); ti<types.end(); ti++) {
      if ( *ti != "stdjob" ) {
	// make a local copy of the post process script  
	string tmpfile = tmpdir + string("/tmp_post_") + (*ti)  + opt_["-jobid"];
#ifdef LOGL3
	cout << "Make a local copy of postprocess script: " << tmpfile << endl;
#endif
	if ( db.getPostProcess((*ti), tmpfile) != 0 ) {
	  cerr << "Unable to get a copy of the post process script. Abort." << endl;
	  return -3;
	}
        // wait at most 5 seconds for processes linked to pipes and then kill
        int maxWait = 5;
	// create post process command on stdout
	if ( sys->fileExist(stdout_file) ) {
	  // create a tmp pipe
	  string post_o_pipe = tmpdir + string("/BossPostPipe-") + sys->convert2string(my_pid);
	  sys->makeFIFO (post_o_pipe, 0600); 
	  // create parser process for stdout
	  BossProcess* postp = new BossExternalProcess(tmpfile,stdout_file,post_o_pipe);
	  sys->forkProcess(postp);
	  BossProcess* labp = new BossLabellerProcess(id,(*ti),post_o_pipe,journal);
	  // read the pipe and update journal file
	  labp->start();
	  delete labp;
	  sys->waitProcessMaxTime(postp,maxWait);
#ifdef LOGL3
	  cout << "STDOUT post process exited with code " << postp->retCode() << endl;
#endif
	  delete postp;
	  // remove tmp pipe
	  sys->fileRemove(post_o_pipe);
#ifdef LOGL3
	  cout << "Removed " << post_o_pipe << endl;
#endif
	} else {
	  cerr << "STDOUT file doesn't exist: " << stdout_file << endl;
	} 
	
	// create post process command on stderr
	if ( sys->fileExist(stderr_file) ) {
	  // create a tmp pipe
	  string post_e_pipe = tmpdir + string("/BossPostPipe-") + sys->convert2string(my_pid);
	  sys->makeFIFO (post_e_pipe, 0600); 
	  // create parser process for stdout
	  BossProcess* postep = new BossExternalProcess(tmpfile,stderr_file,post_e_pipe);
	  sys->forkProcess(postep);
	  BossProcess* labp = new BossLabellerProcess(id,(*ti),post_e_pipe,journal);
	  // read the pipe and update journal file
	  labp->start();
	  delete labp;
	  sys->waitProcessMaxTime(postep,maxWait);
#ifdef LOGL3
	  cout << "STDERR post process exited with code " << postep->retCode() << endl;
#endif
	  delete postep;
	  // remove tmp pipe
	  sys->fileRemove(post_e_pipe);
#ifdef LOGL3
	  cout << "Removed " << post_e_pipe << endl;
#endif
	} else {
	  cerr << "STDERR file doesn't exist: " << stderr_file << endl;
	} 
	
	// remove post-process file
	sys->fileRemove(tmpfile);    
	
	// LOGGING
#ifdef LOGL3
	cout << "Removed " << tmpfile << endl;
#endif
      }
    }
    
    sys->append(journal,"]");

    cerr << "The journal of the recovered information is " << journal << endl;

  }

  // Now update the database
  int ret = jobH->setData(journal);
  if ( ret != 0 )
    cerr << "Error updating the database!" << endl;

  // delete the job handle
  delete jobH;

  return ret;
}

// 1 iff the job is running or idle (not running but scheduled)
// 0 otherwise
int BossRecoverJob::checkRunning(BossJob* jobH, BossDatabase* db) {

  BossScheduler sched(db);
  string state =  sched.query(jobH);  

  if ( state == "unknown" ) {
    return 0;
  } else {
    return 1;
  }
  
}

// 1 iff the user answer y
// 0 otherwise
int BossRecoverJob::prompt() {

  string answer;
  cout << "WARNING all the job information will be overwritten" << endl;
  cout << "Do you want to continue ? ";
  do {
    cout << "y/n ? ";
    cin >> answer;
    cout << endl;
  } while ( answer != "y" && answer != "n" );

  if ( answer == "y" ) 
    return 1;
  else
    return 0;
}

string BossRecoverJob::redirectDir(string base_dir, string dir) {
  
  string outdir;

  if ( dir.size() > 0 ) {
    if ( dir[0] == '/' ) {
      // absolute path
      outdir = dir;
    } else {
      // relative path
      outdir = base_dir + string("/") + dir;
    }
  }

  return outdir;
}

