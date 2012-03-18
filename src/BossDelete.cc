// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossQuery.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <iostream>
#include "BossDelete.h"
#include "BossJob.h"
#include "BossDatabase.h"
#include "BossScheduler.h"

using namespace std;

BossDelete::BossDelete() : BossCommand() {
  opt_["-jobid"] = ""; 
  opt_["-noprompt"] = "FALSE"; 
}

BossDelete::~BossDelete() {}

void BossDelete::printUsage() const
{
  cout << "Usage:" << endl
       << "boss delete " << endl;
  cout << "           -jobid <job id> " << endl
       << "           -noprompt " << endl
       << endl;
}

int BossDelete::execute() {
  //  for (Options_const_iterator i=opt_.begin();i!=opt_.end();i++)
  //    cout << i->first << "=" << i->second << endl;
  
  BossDatabase db("super");
  BossJob* jobH;  

  if ( opt_["-jobid"] == "" ) { 
    return -1;
  }
  // check if the job, scheduler and jobtype exists
  jobH = db.findJob(atoi(opt_["-jobid"].c_str()));
  if ( !jobH ) {
    cout << "JobID " <<  opt_["-jobid"] << " not found" << endl;
    return -3;
  }

  // check if the job is not running 
  if ( checkRunning(jobH, &db) ) {
   cout << "the job is scheduled or running, try later" << endl;
   return -2;
  }

  if ( opt_["-noprompt"] == "FALSE" && !prompt() ) {
    return -4;
  }

  if ( db.deleteJob(jobH->getId()) ) {
    cout << "an error occured while removing job, try later" << endl;
    // at this point do not return, trying to eliminate db inconsistency
  }

  vector<string> types = jobH->getJobTypes();
  for(vector<string>::const_iterator ti=types.begin(); ti<types.end(); ti++) {
    if ( *ti != "stdjob" ) {
      if ( db.deleteSpecificJob((*ti),jobH->getId()) ) {
	cout << "an error occured while removing job specific data, try later" << endl;
	// at this point do not return, trying to eliminate db inconsistency
	if ( !db.existJobType(*ti) ) {
	  cout << "Job type " << *ti
	       << " now not supported ! use boss prune -jobtype to free unused space" << endl;
	  return -5;
	}
      }
    }
  }
  // delete the job handle
  delete jobH;

  return 0;
}


// 1 iff the job is running or idle (not running but scheduled)
// 0 otherwise
int BossDelete::checkRunning(BossJob* jobH, BossDatabase* db) {

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
int BossDelete::prompt() {

  string answer;
  cout << "WARNING the job will be removed" << endl;
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
