// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossKill.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <iostream>
#include "BossKill.h"

#include "BossDatabase.h"
#include "BossScheduler.h"
#include "BossJob.h"

using namespace std;

BossKill::BossKill() : BossCommand() {
  opt_["-jobid"] = "NULL"; 
}

BossKill::~BossKill() {}

void BossKill::printUsage() const
{
  cout << "Usage:" << endl
       << "boss kill " << endl;
  cout << "          -jobid <job ij> " << endl
       << endl;
}

int BossKill::execute() {
  //  for (Options_const_iterator i=opt_.begin();i!=opt_.end();i++)
  //    cout << i->first << "=" << i->second << endl;

  BossDatabase db("super");
  BossJob* jobH;

  if ( opt_["-jobid"] == "NULL" ) { 
    //printUsage();
    return -1;
  }
  // check if the job, scheduler and jobtype exists
  jobH = db.findJob(atoi(opt_["-jobid"].c_str()));
  if ( !jobH ) {
    cout << "JobID " <<  opt_["-jobid"] << " not found" << endl;
    return -3;
  }
  string schedname = jobH->getSchedType();
  if ( schedname != "" && !db.existSchType(schedname) ) {  
    cout << "Job " << jobH->getId() << " was submitted from scheduler " 
	 << jobH->getSchedType() << " now not supported !" << endl;
    return -4;
  }
  
  BossScheduler sched(&db);
  cout << "Return code of kill " << sched.kill(jobH) << endl;

  // delete the job handle
  delete jobH;

  return 0;
}
