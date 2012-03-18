// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossDeclare.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <iostream>

#include "BossDeclare.h"
#include "BossKernel.h"
#include "BossJob.h"

BossDeclare::BossDeclare() : BossCommand() {
  opt_["-jobtype"] = "stdjob"; 
  opt_["-executable"] = "NULL"; 
  opt_["-args"] = ""; 
  opt_["-stdin"] = "/dev/null"; 
  opt_["-stdout"] = "/dev/null"; 
  opt_["-stderr"] = "/dev/null"; 
  opt_["-classad"] = "NULL"; 
  opt_["-log"] = "NULL"; 
}

BossDeclare::~BossDeclare() {}

void BossDeclare::printUsage() const
{
  std::cerr << "Usage:" << std::endl
       << "boss declare " << std::endl
       << "            [ -jobtype <registered job type> (stdjob)" << std::endl
       << "              -executable <executable file to be submitted>" << std::endl
       << "              -args <arguments to the executable>" << std::endl
       << "              -stdin <standard input file>" << std::endl
       << "              -stdout <standard output file>" << std::endl
       << "              -stderr <standard error file>" << std::endl
       << "              -log <scheduler log file> ]" << std::endl
       << "            OR " << std::endl
       << "            [ -classad <classad file> ]" << std::endl
       << std::endl;
}

int BossDeclare::execute() {
  // DEBUG
  // for (Options_const_iterator i=opt_.begin();i!=opt_.end();i++)
  //   std::cout << i->first << "=" << i->second << std::endl;
  // END DEBUG

  BossDatabase db("super");

  BossJob* jobH = Boss::declareJob(opt_,db);
  if ( jobH == 0 ) {
    std::cerr << "Error creating job" << std::endl;
    return -1;
  }
  std::cout << "Job ID " << jobH->getId() << std::endl;
  delete jobH;
  return 0;
}
