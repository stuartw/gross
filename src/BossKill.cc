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
  opt_["-jobid"] = "0:-1";
  opt_["-force"] = "FALSE";
}

BossKill::~BossKill() {}

void BossKill::printUsage() const
{
  cout << "Usage:" << endl
       << "boss kill " << endl;
  cout << "           -jobid [<job id> | <first job id>:<last job id>]" << endl
       << "           -force (set status killed regardless the status)" << endl
       << endl;
}

int BossKill::execute() {
  //  for (Options_const_iterator i=opt_.begin();i!=opt_.end();i++)
  //    cout << i->first << "=" << i->second << endl;

  BossDatabase db("super");

  BossJobIDRange idr(opt_["-jobid"]);

  bool force = opt_["-force"] == "TRUE";

  if ( idr.size()<1 ) {
    std::cerr << "please specify a valid ID range" << std::endl;
    return -1;
  }

  for (int id=idr.ifirst(); id<=idr.ilast(); ++id) {
    // check if the job, scheduler and jobtype exists
    BossJob* jobH = db.findJob(id);
    if ( !jobH )
      continue;

    string schedname = jobH->getSchedType();
    if ( schedname != "" && !db.existSchType(schedname) ) {  
      cout << "Job " << jobH->getId() << " was submitted from scheduler " 
	   << jobH->getSchedType() << " now not supported !" << endl;
      continue;
    }
    
    BossScheduler sched(&db);
    cout << "Return code of kill " << sched.kill(jobH,force) << endl;
    
    // delete the job handle
    delete jobH;
  }
  
  return 0;
}
