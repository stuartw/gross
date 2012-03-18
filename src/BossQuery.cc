// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossQuery.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <iostream>
#include "BossQuery.h"
#include "BossJob.h"
#include "BossDatabase.h"
#include "BossScheduler.h"
#include <unistd.h>
#include <cstdio>

using namespace std;

BossQuery::BossQuery() : BossCommand() {
  opt_["-jobid"] = ""; 
  opt_["-all"] = "FALSE"; 
  opt_["-scheduled"] = "TRUE"; 
  opt_["-running"] = "FALSE"; 
  opt_["-type"] = "";
  opt_["-user"] = "";
  opt_["-specific"] = "FALSE"; 
  opt_["-full"] = "FALSE"; 
  opt_["-statusOnly"] = "FALSE"; 
}

BossQuery::~BossQuery() {}

void BossQuery::printUsage() const
{
  cout << "Usage:" << endl
       << "boss query " << endl;
  cout << "           -jobid <job id> " << endl
       << "           -all " << endl
       << "           -scheduled " << endl
       << "           -running " << endl
       << "           -type <job type>" << endl
       << "           -user <user>" << endl
       << "           -specific " << endl
       << "           -full " << endl
       << "           -statusOnly (overrides -specific and -full)" << endl
       << endl;
}

int BossQuery::execute() {
  
  BossDatabase db("standard");
  BossScheduler sched(&db);

  //  for (Options_const_iterator i=opt_.begin();i!=opt_.end();i++)
  //  cout << i->first << "=" << i->second << endl;

  string projection_opt = "normal";
  if ( opt_["-specific"] == "TRUE" )
    projection_opt = "specific";
  if ( opt_["-statusOnly"] == "TRUE" )
    projection_opt = "status";

  // specific option is allowed with -type option only 
  if ( projection_opt == "specific" && opt_["-jobid"] == "" && opt_["-type"] == "" ) {
    cout << "Cannot use -specific without -type" << endl;
    return -1;
  }

  if ( opt_["-jobid"] != "" ) {
    if ( opt_["-type"] != "" ) {
      cout << "Cannot use -jobid and -type together" << endl
	   << "Ignoring -type option" << endl;
    }
    // check if the job, scheduler and jobtype exists
    BossJob* jobH = db.findJob(atoi(opt_["-jobid"].c_str()));
    if ( !jobH ) {
      cout << "JobID " <<  opt_["-jobid"] << " not found" << endl;
      return -3;
    }
    if ( opt_["-full"] == "TRUE" && opt_["-statusOnly"] != "TRUE") {
      jobH->dumpGeneral();
      if (projection_opt=="specific")
	jobH->dumpSpecific();
      return 0;
    } else {
      printJobHead(jobH,projection_opt);
      printJob(jobH,projection_opt,getState(jobH,sched),&db);
      return 0;
    }
  } else {
    char filter_option = ' ';
    if ( opt_["-all"] != "FALSE" ) {
      filter_option = 'a';
      opt_["-scheduled"] = "FALSE";
    } else if ( opt_["-running"] != "FALSE" ) {
      filter_option = 'r';
      opt_["-scheduled"] = "FALSE";
    } else if ( opt_["-scheduled"] != "FALSE" ) {
      filter_option = 's';
    } else {
      return -1;
    }

    // find the jobs matching
    vector<BossJob*> jobs = db.jobs(filter_option,opt_["-type"],opt_["-user"],projection_opt);

    int matching_jobs = 0;
    bool printHead = 1;
    for (vector<BossJob*>::const_iterator i=jobs.begin(); i!=jobs.end();i++) { 
      if(printHead){
	printJobHead((*i),projection_opt);
	printHead = 0;
      }
      string state = getState(*i,sched);
      if ( filter_option != 'a' ) {
	if ( state != "A") { 
	  printJob((*i),projection_opt,state,&db);
	  matching_jobs++;
	}
      } else {
	printJob((*i),projection_opt,state,&db);
	matching_jobs++;
      }
      delete (*i);
    }
    if ( projection_opt != "status" ) {
      cout << "Jobs matching : " << matching_jobs << endl;
    }
  }

  return 0;
}

int BossQuery::printJobHead(BossJob* jobH, string opt) {

  if ( opt != "status" ) {
    jobH->printGeneral("header_normal");
    if ( opt == "specific" )jobH->printSpecific("header_normal");
    cout << endl;
  }

  return 0;

}

int BossQuery::printJob(BossJob* jobH, string opt, string state, 
			BossDatabase* db) {

  string schedname = jobH->getSchedType();
  if ( schedname != "" && !db->existSchType(schedname) ) {
    cout << "Job " << jobH->getId() << " was submitted from scheduler " 
	 << schedname << " now not supported !" << endl;
    return -4;
  }

  if ( (opt_["-scheduled"] != "FALSE" || opt_["-running"] != "FALSE") ) {
    if  ( state != "A" ) {
      if ( opt == "status" ) {
	cout << jobH->getId() << " " << state << endl;
      } else {
	jobH->printGeneral("normal",state);
	if ( opt == "specific" )jobH->printSpecific("normal");
	cout << endl;
      }
    } else {
    }
  } else {
    if ( opt == "status" ) {
      cout << jobH->getId() << " " << state << endl;
    } else {
      jobH->printGeneral("normal",state);
      if ( opt == "specific" )jobH->printSpecific("normal");
      cout << endl;
    }
  }
  
  return 0;
}

// return the state of the job
string BossQuery::getState(BossJob* jobH, BossScheduler& sched) {
  return sched.status(jobH);
}
