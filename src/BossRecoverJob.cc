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
#include "OperatingSystem.h"
#include "BossJob.h"
#include "Process.h"
#include "ExternalProcess.h"
#include "BossLabellerProcess.h"
#include "BossJobIDRange.h"

using namespace std;

BossRecoverJob::BossRecoverJob() : BossCommand() {
  opt_["-jobid"] = "0:-1";
  opt_["-noprompt"] = "FALSE"; 
  opt_["-localIO"] = "FALSE";
  opt_["-journal"] = "";
  opt_["-journaldir"] = "NONE";
}

BossRecoverJob::~BossRecoverJob() {}

void BossRecoverJob::printUsage() const
{
  cout << "Usage:" << endl
       << "boss recoverJob " << endl;
  cout << "           -jobid [<job id> | <first job id>:<last job id>]" << endl
       << "           -noprompt " << endl
       << "           -localIO " << endl
       << "           -journaldir <dir>" << endl
       << endl;
}

int BossRecoverJob::execute() {

  //  for (Options_const_iterator i=opt_.begin();i!=opt_.end();i++)
  //    cout << i->first << "=" << i->second << endl;
  
  BossDatabase db("super");

  int my_pid = getpid();
  BossConfiguration* config=BossConfiguration::instance();
  string tmpdir = config->boss_tmp_dir();

  BossJobIDRange idr(opt_["-jobid"]);

  if ( idr.size()<1 ) {
    std::cerr << "please specify a valid ID range" << std::endl;
    return -1;
  }

  int ret=0;
  
  for (int id=idr.ifirst(); id<=idr.ilast(); ++id) {
    // check if the job exists
    BossJob* jobH = db.findJob(id);
    if ( !jobH )
      continue;

    // check if the job is not running 
    if ( checkRunning(jobH, &db) ) {
      cerr << "job " << id << " is either scheduled or running" << endl
	   << "please wait until it finishes or kill it" << endl;
      continue;
    }
    
    if ( opt_["-noprompt"] == "FALSE" && !prompt(id) )
      continue;

    std::string journal="";
    // Recover from journal
    if (opt_["-journaldir"] != "NONE" ) {
      journal = opt_["-journaldir"]+
	"/BossJournal_"+OSUtils::convert2string(id)+".txt";
      if (!OSUtils::fileExist(journal)) {
	cerr << "Journal file not found: " << journal << endl;
	++ret;
	delete jobH;
	continue;
      }
    } else {    
      // Try to re-run the post-process script on STDOUT/ERR
      journal = std::string("BossJournal_Recovered_")+OSUtils::convert2string(id)+".txt";
      //try first with a local journal:
      if ( OSUtils::touch(journal) != 0 ) {
	// in case of error try with a journal in boss temporary dir:
	journal = tmpdir + "/" + journal;
	if ( OSUtils::touch(journal) != 0 ) {
	  cerr << "Unable to create a local journal file " << journal << endl;
	  ++ret;
	  delete jobH;
	  continue;
	}
      }
      OSUtils::append(journal,"[");
      
      // Get job parameters
      string old_dir=jobH->getSubPath();
      string stdOutput=jobH->getStdout();
      string stdError=jobH->getStderr();
      // adjust file with absolute path
      string stdout_file = redirectDir(old_dir, stdOutput);
      string stderr_file = redirectDir(old_dir, stdError);
      if ( opt_["-localIO"] == "TRUE" ) {
	if(stdout_file != "/dev/null")stdout_file = OSUtils::basename(stdout_file);
	if(stderr_file != "/dev/null")stderr_file = OSUtils::basename(stderr_file);
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
	    ++ret;
	    delete jobH;
	    continue;
	  }
	  // wait at most 5 seconds for processes linked to pipes and then kill
	  int maxWait = 5;
	  // create post process command on stdout
	  if ( OSUtils::fileExist(stdout_file) ) {
	    // create a tmp pipe
	    string post_o_pipe = tmpdir + string("/BossPostPipe-") + OSUtils::convert2string(my_pid);
	    OSUtils::makeFIFO (post_o_pipe, 0600); 
	    // create parser process for stdout
	    OSUtils::Process* postp = new OSUtils::ExternalProcess(tmpfile,stdout_file,post_o_pipe);
	    OSUtils::forkProcess(postp);
	    OSUtils::Process* labp = new BossLabellerProcess(id,(*ti),post_o_pipe,journal);
	    // read the pipe and update journal file
	    labp->start();
	    delete labp;
	    OSUtils::waitProcessMaxTime(postp,maxWait);
#ifdef LOGL3
	    cout << "STDOUT post process exited with code " << postp->retCode() << endl;
#endif
	    delete postp;
	    // remove tmp pipe
	    OSUtils::fileRemove(post_o_pipe);
#ifdef LOGL3
	    cout << "Removed " << post_o_pipe << endl;
#endif
	  } else {
	    cerr << "STDOUT file doesn't exist: " << stdout_file << endl;
	  } 
	  
	  // create post process command on stderr
	  if ( OSUtils::fileExist(stderr_file) ) {
	    // create a tmp pipe
	    string post_e_pipe = tmpdir + string("/BossPostPipe-") + OSUtils::convert2string(my_pid);
	    OSUtils::makeFIFO (post_e_pipe, 0600); 
	    // create parser process for stdout
	    OSUtils::Process* postep = new OSUtils::ExternalProcess(tmpfile,stderr_file,post_e_pipe);
	    OSUtils::forkProcess(postep);
	    OSUtils::Process* labp = new BossLabellerProcess(id,(*ti),post_e_pipe,journal);
	    // read the pipe and update journal file
	    labp->start();
	    delete labp;
	    OSUtils::waitProcessMaxTime(postep,maxWait);
#ifdef LOGL3
	    cout << "STDERR post process exited with code " << postep->retCode() << endl;
#endif
	    delete postep;
	    // remove tmp pipe
	    OSUtils::fileRemove(post_e_pipe);
#ifdef LOGL3
	    cout << "Removed " << post_e_pipe << endl;
#endif
	  } else {
	    cerr << "STDERR file doesn't exist: " << stderr_file << endl;
	  } 
	  
	  // remove post-process file
	  OSUtils::fileRemove(tmpfile);    
	  
	  // LOGGING
#ifdef LOGL3
	  cout << "Removed " << tmpfile << endl;
#endif
	}
      }
      
      OSUtils::append(journal,"]");
      
      cerr << "The journal of the recovered information is " << journal << endl;
      
    }
    
    // Now update the database
    ret += jobH->setData(journal);
    if ( ret != 0 )
      cerr << "Error updating the database!" << endl;
    
    // delete the job handle
    delete jobH;
  }

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
int BossRecoverJob::prompt(int id) {

  string answer;
  cout << "WARNING: all job " << id << " information will be overwritten" << endl;
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

