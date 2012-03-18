// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossScheduler.cpp
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <unistd.h>

#include "pstream.h"

#include "BossScheduler.h"

#include "BossConfiguration.h"

#include "BossOperatingSystem.h"
#include "BossDatabase.h"
#include "BossJob.h"

using namespace std;

BossScheduler::BossScheduler(BossDatabase* db) : db_(db), MAXDLY(30), time_(0) {
}

BossScheduler::~BossScheduler() {
  clear();  
}

int BossScheduler::submit(BossJob* jobH, string exe_host) {

  int ret_val = 0;

  string sid, dsl;
  string command;
  string log;

  int jobID = jobH->getId();
  BossOperatingSystem* sys=BossOperatingSystem::instance();

  BossConfiguration* config=BossConfiguration::instance();
  string tmpdir = config->boss_tmp_dir();

  log = jobH->getLog();
  if ( log == "NULL" || log == "") {
     // setup the default log name
     string fnam = jobH->getExecutable() + "_" +  sys->convert2string(jobID);
     log=string(jobH->getSubPath())+string("/")+sys->basename(fnam)+string(".log");
  } else {
    // check if is a relative path
    if ( log[0] != '/' ) {
      log=string(jobH->getSubPath())+string("/")+log;
    }
  }

  // LOGGING
  ofstream l(log.c_str());
#ifdef LOGL1
  l << "======> Scheduler submit on " << sys->getStrTime() << endl;
  l.sync_with_stdio(true);
#endif

  cout << "Write log to " << log << endl;
  jobH->setLog(log);
  
  // make a local copy of the submit script  
  string filename = tmpdir + string("/tmp_submit") + sys->convert2string(jobID);
  string name = jobH->getSchedType();
  int err = -999;
  err = db_->getSubmit(name, filename);
  if        ( err <0 ) {
    cerr << "Unable to get a copy of the submit script for " 
	 << name << ". Abort." << endl;
    return -3;
  } else if (err >0 ) {
    cerr << "Submit script for " 
	 << name << " was not registered. Abort." << endl;
    return -2;
  }

  string top_w_dir = db_->getWorkDir(name);

  // prepare the submission command
  command = filename + " " + sys->convert2string(jobID) + " " + jobH->getLog() + " " 
    + exe_host + " " +top_w_dir;

#ifdef LOGL3
  l << "Running " << command << endl;
  l.sync_with_stdio(true);
#endif

  // execute the submission command
  redi::ipstream psub(command.c_str());
  psub >> sid;
  cout << "Scheduler ID is " << sid << endl;
  psub.close();
  
#ifdef LOGL2
  // cout << "Return of fflush" << fflush(NULL) << endl;
  l << "Scheduler ID " << sid << endl;
  l.sync_with_stdio(true);
#endif
  
  if ( sid != "error" ) {
    // update the transient copy of job
    jobH->setSid(sid);
    jobH->setSubTime(sys->getTime());
  } else {
    cerr << "Job not submitted" << endl;
    ret_val = -1;
  }
  l.sync_with_stdio(true);
  l.flush();
  l.close();
  sys->fileRemove(filename);

  return ret_val;
}

int BossScheduler::kill(BossJob* jobH) {

  BossOperatingSystem* sys=BossOperatingSystem::instance();

  BossConfiguration* config=BossConfiguration::instance();
  string tmpdir = config->boss_tmp_dir();

  string status = "";

  if ( jobH->getSid() == "" ) {
    cout << "Scheduler ID not available. " << endl
	 << "Either the job was not submitted " << endl
	 << "or there was some problem when storing the data. "
	 << endl;
    status = "killed";
  } else {
    if ( !db_->existSchType(jobH->getSchedType()) ) {
      cout << "Scheduler " << jobH->getSchedType()
	   << " now not supported !" << endl;
      return -4;
    }
    // make a local copy of the kill script  
    string filename = tmpdir + string("/tmp_kill") + sys->convert2string(jobH->getId());
    string name = jobH->getSchedType();
    int err = -999;
    err = db_->getKill(name, filename);
    if        ( err <0 ) {
      cerr << "Unable to get a copy of the kill script for " 
	   << name << ". Abort." << endl;
      return -3;
    } else if (err >0 ) {
      cerr << "Kill script for " 
	   << name << " was not registered. Abort." << endl;
      return -2;
    }

    string command = filename + " " + jobH->getSid();
    // cout << "Kill command :" << command << endl;
    redi::ipstream psub(command.c_str());
    psub >> status;
    cout << "Kill status " << status << endl;
    psub.close();
    sys->fileRemove(filename);
  }

  // update the transient copy of job
  if ( status == "killed" ) {
    // set negative time to inform this job was killed
    jobH->setStopTime(-sys->getTime());
    time_t tt = jobH->getStopTime();
    cout << ctime(&tt);
    return 0;
  } else
    return -1;

}

string BossScheduler::query(BossJob* jobH) {

  check();
  string search = jobH->getSchedType()+":#"+jobH->getSid();
  string status = "unknown";
  // DEBUG
  //  cout << "search string: " << search << endl;
  // END DEBUG
  Jobs::const_iterator i = jobs_.find(search);
  if (i != jobs_.end())
    status = (*i).second;
  return status;
}


// W:          the job is waiting for submission
// I:          the job is submitted but is not running (idle)
// R:          the job is running
// E:          the job is finished
// K:          the job was killed by user
// A:          the job aborted
// NA:         not available
string BossScheduler::status(BossJob* jobH) {
  check();
  const time_t null = 0; // time was never set
  string res = "NA";
  // Get sub, start and stop times
  time_t subt = jobH->getSubTime();
  time_t startt = jobH->getStartTime();
  time_t endt = jobH->getStopTime();
  // Known cases
  if ( subt == null && startt == null && endt == null ) {
    // the job is declared but not submitted
    res = "W";
  } else if ( endt < 0 ) {
    // the job is killed while was running
    res = "K";
  } else if ( subt != null && startt != null && endt != null ) {
    // the job is ended
    res = "E";
  } else if ( subt != null && endt == null ) { 
    // the job is idle or running or aborted
    string state = query(jobH);
    if ( state == "unknown" ) {
      // the job was abort
      res = "A";
    } else {
      res = state;
    }
  }
  // others cases are not known
  return res;
}

void BossScheduler::check() {

  BossOperatingSystem* sys=BossOperatingSystem::instance();

  BossConfiguration* config=BossConfiguration::instance();
  string tmpdir = config->boss_tmp_dir();

  time_t current = sys->getTime();
  if(current-time_>MAXDLY) {
    // DEBUG
    // cout << "Interrogating local schedulers " << endl;
    // END DEBUG
    clear();
    vector<string> sched = db_->schedulers();
    for (vector<string>::const_iterator i=sched.begin(); i!=sched.end();i++) { 
      string sch=(*i);
      string status, sid;

      // make a local copy of the query script  
      string filename = tmpdir + string("/tmp_query") + sys->convert2string(sys->getPid());
      int err = -999;
      err = db_->getQuery(sch, filename);
      if        ( err <0 ) {
	cerr << "Unable to get a copy of the query script for " 
	     << sch << ". Abort." << endl;
	return;
      } else if (err >0 ) {
	cerr << "Query script for " 
	     << sch << " was not registered. Abort." << endl;
	return;
      }

      // execute the submission script
      string command = filename;
      //      cout << command << endl;
      redi::ipstream psub(command.c_str());
      while(psub >> sid >> status) {
	jobs_[sch+":#"+sid]=status;
	// DEBUG
	// cout << "map string:   " << sch+":#"+sid << " " << status << endl;
	// END DEBUG
      }
      psub.close();
      sys->fileRemove(filename);
    }
    time_=sys->getTime();
  }
}

void BossScheduler::clear() {
  Jobs_iterator i;
  for (i=jobs_.begin(); i!= jobs_.end(); i++) {
    jobs_.erase(i);
  }
}




