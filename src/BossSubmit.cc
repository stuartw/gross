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
#include "OperatingSystem.h"
#include "BossDatabase.h"
#include "BossScheduler.h"

BossSubmit::BossSubmit() : BossCommand() {
  opt_["-jobid"] = "0:-1";
  opt_["-jobtype"] = "stdjob"; 
  opt_["-scheduler"] = "NULL"; 
  opt_["-executable"] = "NULL"; 
  opt_["-args"] = ""; 
  opt_["-stdin"] = "/dev/null"; 
  opt_["-stdout"] = "/dev/null"; 
  opt_["-stderr"] = "/dev/null"; 
  opt_["-infiles"] = "";
  opt_["-outfiles"] = "";
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
       << "           -scheduler <registered scheduler>" << std::endl
       << "           -host <hostname>" << std::endl
       << "           [ -jobtype <registered job type> (stdjob)" << std::endl
       << "             -executable <executable file to be submitted>" << std::endl
       << "             -args <arguments to the executable>" << std::endl
       << "             -stdin <standard input file>" << std::endl
       << "             -stdout <standard output file>" << std::endl
       << "             -stderr <standard error file>" << std::endl
       << "             -infiles <coma-separated file list in input> (AKA input sandbox)" << std::endl
       << "             -outfiles <coma-separated file list in output> (AKA output sandbox)" << std::endl
       << "             -log <scheduler log file> ]" << std::endl
       << "           OR " << std::endl
       << "           [ -classad <classad file> ]" << std::endl
       << "           OR " << std::endl
       << "           [ -jobid [<job id> | <first job id>:<last job id>]" << std::endl
       << std::endl;
}

int BossSubmit::execute() {
  //  for (Options_const_iterator i=opt_.begin();i!=opt_.end();i++)
  //    cout << i->first << "=" << i->second << std::endl;

  BossDatabase db("super");

  BossJobIDRange idr(opt_["-jobid"]);

  // if the job was not previously declared (no -jobid option), do it.
  if ( idr.size()<1 ) {  
    BossJob* jobH = Boss::declareJob(opt_,db);
    if ( !jobH ) {
      std::cerr << "Error creating job" << std::endl;
      return -1;
    }
    idr.updateRange(OSUtils::convert2string(jobH->getId()));
    delete jobH;
  }

  // check if the scheduler exists
  if ( opt_["-scheduler"]!="NULL" && !db.existSchType(opt_["-scheduler"]) ) {
    std::cerr << "Scheduler not known: " << opt_["-scheduler"] << std::endl;
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
  
  int ret_val = 0;
  // loop over jobs
  for (int id=idr.ifirst(); id<=idr.ilast(); ++id) {
    std::string strid = OSUtils::convert2string(id);
    BossJob* jobH = db.findJob(id);
    std::cout << "Job ID " << id << std::endl;
    // go to the direcory where the job was declared
    std::string subdir = jobH->getSubPath();
    OSUtils::changeDir(subdir);
    // Submit the job 
    int ret_val = 0;
    jobH->setScheduler(opt_["-scheduler"]);
    BossScheduler sched(&db);
    if ( sched.submit(jobH, opt_["-host"]) != 0 ) {
      std::cerr << "Unable to submit job" << std::endl;
      ++ret_val;
      continue;
    }
    // Remove the temporary ClassAd file used by the submit script
    std::string ocladfname = std::string("BossClassAdFile_") + OSUtils::convert2string(id);
    if ( OSUtils::fileExist(ocladfname) ) 
      OSUtils::fileRemove(ocladfname);
    delete jobH;
  }
  return ret_val;
}
