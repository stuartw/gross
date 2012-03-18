// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossSubmit.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>

#include "BossSubmit.h"
#include "BossKernel.h"
#include "BossJob.h"
#include "BossOperatingSystem.h"
#include "BossDatabase.h"
#include "BossScheduler.h"

BossSubmit::BossSubmit() : BossCommand() {
  opt_["-jobid"] = "NULL"; 
  opt_["-jobtype"] = "stdjob"; 
  opt_["-scheduler"] = "NULL"; 
  opt_["-executable"] = "NULL"; 
  opt_["-args"] = ""; 
  opt_["-stdin"] = "/dev/null"; 
  opt_["-stdout"] = "/dev/null"; 
  opt_["-stderr"] = "/dev/null"; 
  opt_["-classad"] = "NULL"; 
  opt_["-host"] = "NULL";
  opt_["-log"] = "NULL"; 
}

BossSubmit::~BossSubmit() {}

void BossSubmit::printUsage() const
{
  std::cerr << "Usage:" << std::endl
       << "boss submit " << std::endl;
  std::cerr
       << "            -scheduler <registered scheduler>" << std::endl
       << "            -host <hostname>" << std::endl
       << "            [ -jobtype <registered job type> (stdjob)" << std::endl
       << "              -executable <executable file to be submitted>" << std::endl
       << "              -args <arguments to the executable>" << std::endl
       << "              -stdin <standard input file>" << std::endl
       << "              -stdout <standard output file>" << std::endl
       << "              -stderr <standard error file>" << std::endl
       << "              -log <scheduler log file> ]" << std::endl
       << "            OR " << std::endl
       << "            [ -classad <classad file> ]" << std::endl
       << "            OR " << std::endl
       << "            [ -jobid <declared job identifier> ]" << std::endl
       << std::endl;
}

int BossSubmit::execute() {
  //  for (Options_const_iterator i=opt_.begin();i!=opt_.end();i++)
  //    cout << i->first << "=" << i->second << std::endl;

  BossDatabase db("super");

  BossJob* jobH = 0;
  if ( opt_["-jobid"] != "NULL" ) {    
    // If the -jobid option is used, try to get the job handle from the DB
    jobH = db.findJob(atoi(opt_["-jobid"].c_str()));
    if ( !jobH ) {
      std::cerr << "JobID " <<  opt_["-jobid"] << " not found" << std::endl;
      return -1;
    }
  } else {
    // Create an entry in the db for the job
    jobH = Boss::declareJob(opt_,db);
    if ( !jobH ) {
      std::cerr << "Error creating job" << std::endl;
      return -2;
    }
  }
  int id = jobH->getId();
  std::cout << "Job ID " << id << std::endl;

  // check if the scheduler exists
  if ( opt_["-scheduler"]!="NULL" && !db.existSchType(opt_["-scheduler"]) ) {
    std::cerr << "Scheduler not known: " << opt_["-jobtype"] << std::endl;
    return -3;
  } else if ( opt_["-scheduler"]=="NULL" ) {
    // get the default scheduler
    std::cout << "look for default scheduler" << std::endl;
    opt_["-scheduler"] = db.getDefaultSch();
    if (  opt_["-scheduler"] == "NULL" ) {
      std::cerr << "Default Scheduler not set!" << std::endl;
      return -4;
    }
  }
  jobH->setScheduler(opt_["-scheduler"]);

  // Submit the job
  int ret_val = 0;
  BossScheduler sched(&db);
  if ( sched.submit(jobH, opt_["-host"]) != 0 ) {
    std::cerr << "Unable to submit job" << std::endl;
    ret_val = -5;
  }
  // Remove the temporary ClassAd file used by the submit script
  BossOperatingSystem* sys=BossOperatingSystem::instance();
  std::string ocladfname = std::string("BossClassAdFile_") + sys->convert2string(id);
  if ( sys->fileExist(ocladfname) ) 
    sys->fileRemove(ocladfname);
  delete jobH;
  return ret_val;
}
