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

#include "OperatingSystem.h"

#include "BossScheduler.h"

#include "BossConfiguration.h"

#include "BossDatabase.h"
#include "BossJob.h"

using namespace std;

BossScheduler::BossScheduler(BossDatabase* db) : db_(db), MAXDLY(60), DELIM(":#") {
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

  BossConfiguration* config=BossConfiguration::instance();
  string tmpdir = config->boss_tmp_dir();

  log = jobH->getLog();
  if ( log == "NULL" || log == "") {
     // setup the default log name
     string fnam = jobH->getExecutable() + "_" +  OSUtils::convert2string(jobID);
     log=string(jobH->getSubPath())+string("/")+OSUtils::basename(fnam)+string(".log");
  } else {
    // check if is a relative path
    if ( log[0] != '/' ) {
      log=string(jobH->getSubPath())+string("/")+log;
    }
  }

  // LOGGING
  ofstream l(log.c_str());
#ifdef LOGL1
  l << "======> Scheduler submit on " << OSUtils::getStrTime() << endl;
  l.sync_with_stdio(true);
#endif

  cout << "Write log to " << log << endl;
  jobH->setLog(log);
  
  // make a local copy of the submit script  
  string filename = tmpdir + string("/tmp_submit") + OSUtils::convert2string(jobID);
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
  string copy_comm = db_->getCopy(name);

  // prepare the submission command
  command = filename + " " + OSUtils::convert2string(jobID) + " " + jobH->getLog() + " " 
    + exe_host + " " +top_w_dir+" "+copy_comm;
  

#ifdef LOGL3
  l << "Running " << command << endl;
  l.sync_with_stdio(true);
#endif

  // execute the submission command
  OSUtils::CommandStream psub(command.c_str());
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
    jobH->setSubTime(OSUtils::getTime());
  } else {
    cerr << "Job not submitted" << endl;
    ret_val = -1;
  }
  l.sync_with_stdio(true);
  l.flush();
  l.close();
  OSUtils::fileRemove(filename);

  return ret_val;
}

int BossScheduler::kill(BossJob* jobH, bool force) {

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
    string filename = tmpdir + string("/tmp_kill") + OSUtils::convert2string(jobH->getId());
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
    OSUtils::CommandStream psub(command.c_str());
    psub >> status;
    cout << "Kill status " << status << endl;
    psub.close();
    OSUtils::fileRemove(filename);
  }

  // update the transient copy of job
  if ( status == "killed" || force ) {
    // set negative time to inform this job was killed
    jobH->setStopTime(-OSUtils::getTime());
    time_t tt = jobH->getStopTime();
    cout << ctime(&tt);
    return 0;
  } else
    return -1;

}

string BossScheduler::query(BossJob* jobH) {

  check(jobH->getSchedType());
  string search = jobH->getSchedType()+DELIM+jobH->getSid();
  string status = "unknown";
  // DEBUG
  //  cout << "search string: " << search << endl;
  // END DEBUG
  Jobs::const_iterator i = jobs_.find(search);
  if (i != jobs_.end())
    status = (*i).second;
  return status;
}


// W:          not submitted
// I:          submitted, not started, the scheduler reports idle
// R:          submitted, started, with recent time stamp
// E:          ended
// K:          killed by user
// A:          submitted, not known to scheduler
// I?:         submitted, not started no check of scheduler
// A?:         submitted, started, no recent time stamp. no check of scheduler
// TO:         time out: no recent time stamp but scheduler reports runnning
string BossScheduler::status(BossJob* jobH, bool flag) {
  const time_t null = 0; // time was never set
  string res = "NA";
  BossConfiguration* config=BossConfiguration::instance();
  // Get sub, start and stop times
  time_t subt = jobH->getSubTime();
  time_t startt = jobH->getStartTime();
  time_t endt = jobH->getStopTime();
  time_t lastt = jobH->getLastContactTime();
  time_t now = OSUtils::getTime();
  // Known cases
  if ( subt == null && startt == null && endt == null ) {
    // the job is declared but not submitted
    res = "W";
  } else if ( endt < 0 ) {
    // the job is killed while was running
    res = "K";
  } else if ( endt > 0 ) {
    // the job is ended
    res = "E";
  } else if (now-lastt < config->boss_max_upd_int()) { 
    // the job has a recent time stamp. Report as running.
    res = "R";
  } else if ( flag ) {
    // Avoid queying the scheduler
    if ( startt == 0 ) {
      // the job didn't start yet. Assume it is in queue
      res = "I?";
    } else {
      // the job started but is not updating. Assume it is aborted
      res = "A?";
    }
  } else {
    // Query the scheduler
    string state = query(jobH);
    if ( state == "unknown" ) {
      // the job was abort
      res = "A";
    } else if (state == "R") {
      // the scheduler reports it's running but it is timing out
      res = "TO";
    } else {
      res = state;
    }
  }
  // others cases are not known
  return res;
}

void BossScheduler::check(std::string sch) {
  // DEBUG
  // std::cerr << "BossScheduler::check(" << sch << ")" << endl;
  // ENDDEBUG
  if (sch == "")
    return;
  if (!db_->existSchType(sch)) {
    std::cerr << "Scheduler " << sch << " does not exist" << endl;
    return;
  }
  time_t lastTime = 0;
  LTM_iterator it = lastTimes_.find(sch);
  if ( it != lastTimes_.end() )
    lastTime = it->second;
  time_t current = OSUtils::getTime();
  if(current-lastTime>MAXDLY) {
    BossConfiguration* config=BossConfiguration::instance();
    string tmpdir = config->boss_tmp_dir();
    clear(sch);
    string status, sid;
    // make a local copy of the query script  
    string filename = tmpdir + string("/tmp_query") + OSUtils::convert2string(OSUtils::getPid());
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
    OSUtils::CommandStream psub(command.c_str());
    while(psub >> sid >> status) {
      jobs_[sch+DELIM+sid]=status;
      // DEBUG
      // cout << "map string:   " << sch+DELIM+sid << " " << status << endl;
      // END DEBUG
    }
    psub.close();
    OSUtils::fileRemove(filename);
  }
  lastTimes_[sch]=OSUtils::getTime();
}

void BossScheduler::clear(string sch) {
  // DEBUG
  // std::cerr << "BossScheduler::clear(" << sch << ")" << std::endl;
  // ENDDEBUG
  Jobs_iterator i;
  for (i=jobs_.begin(); i!= jobs_.end(); i++) {
    if ( sch.size()==0 || 
	 sch==(i->first).substr(0,(i->first).find(DELIM)))
      jobs_.erase(i);
  }
}




