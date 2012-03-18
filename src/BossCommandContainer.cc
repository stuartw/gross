// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossCommandContainer.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include "BossCommandContainer.h"

#include "BossVersion.h"
#include "BossSubmit.h"
#include "BossDeclare.h"
#include "BossKill.h"
#include "BossQuery.h"
#include "BossRegisterJob.h"
#include "BossRegisterScheduler.h"
#include "BossShowSchedulers.h"
#include "BossShowJobTypes.h"
#include "BossSQL.h"
#include "BossDelete.h"
#include "BossPurge.h"
#include "BossRecoverJob.h"
#include "BossDeleteJobType.h"
#include "BossDeleteScheduler.h"
#include "BossConfigureDB.h"

using namespace std;

BossCommandContainer::BossCommandContainer() {
  // All the command interfaces are created here
  // allow aliases for commands
  BossCommand* c;
  // Version
  c=new BossVersion();
  cmd_["version"]=c;
  cmd_["v"]=c;
  // Declare
  c=new BossDeclare();
  cmd_["declare"]=c;
  // Submit
  c=new BossSubmit();
  cmd_["submit"]=c;
  cmd_["s"]=c;
  // Kill
  c=new BossKill();
  cmd_["kill"]=c;
  cmd_["k"]=c;
  // Query
  c=new BossQuery();
  cmd_["query"]=c;
  cmd_["q"]=c;
  // Register a Scheduler
  c=new BossRegisterScheduler();
  cmd_["registerScheduler"]=c;
  // Register a job type
  c=new BossRegisterJob();
  cmd_["registerJob"]=c;
  // Show registered schedulers
  c=new BossShowSchedulers();
  cmd_["showSchedulers"]=c;
  // Show registered job types
  c=new BossShowJobTypes();
  cmd_["showJobTypes"]=c;
  // SQL query
  c=new BossSQL();
  cmd_["SQL"]=c;
  // Purge old database entries
  c=new BossPurge();
  cmd_["purge"]=c;
  cmd_["p"]=c;
  // Delete a database job entrie
  c=new BossDelete();
  cmd_["delete"]=c;
  cmd_["d"]=c;
  // Rerun postprocess on a given job
  c=new BossRecoverJob();
  cmd_["recoverJob"]=c;
  // Delete a job type
  c=new BossDeleteJobType();
  cmd_["deleteJobType"]=c;
  // Delete a scheduler
  c=new BossDeleteScheduler();
  cmd_["deleteScheduler"]=c;
  // configure the database
  c=new BossConfigureDB();
  cmd_["configureDB"]=c;
}

BossCommandContainer::~BossCommandContainer() {
  /*
  for(Commands_iterator i=cmd_.begin();i!=cmd_.end();i++) {
    if(i->second){ // do not delete twice the same command (allow for aliases)
      delete i->second;
      i->second=0;
    }
  }
  */
}

BossCommand* BossCommandContainer::command(const string s) const {
  Commands_const_iterator ci = cmd_.find(s);
  if(ci==cmd_.end())
    return 0;
  else
    return ci->second;
}

void BossCommandContainer::printUsage() const 
{
  cout << "Usage:" << endl << "boss command [comand options]" << endl;
  cout << "command:" << endl
       << "\tversion [v]          : print boss version string" << endl
       << "\tregisterScheduler    : register a new scheduler" << endl
       << "\tregisterJob          : register a new type of job" << endl
       << "\tshowSchedulers       : list registered schedulers" << endl
       << "\tshowJobTypes         : list registered jobtypes" << endl
       << "\tdeleteScheduler      : delete a scheduler" << endl
       << "\tdeleteJobType        : delete a jobtype" << endl
       << "\tdeclare              : declare a job without submitting" << endl
       << "\tsubmit  [s]          : submit a job" << endl
       << "\tquery   [q]          : get info about one or more jobs" << endl
       << "\tSQL                  : SQL query" << endl
       << "\tkill    [k]          : kill a job" << endl
       << "\trecoverJob           : update job info on the DB from journal" << endl
       << "\t                       or from job STDOUT/ERR" << endl
       << "\tdelete  [d]          : delete job entries from the DB" << endl
       << "\tpurge   [p]          : purge old job ebtries from the DB" << endl;
}







