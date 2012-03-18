// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossCommand.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include "BossCommand.h"

using namespace std;

BossCommand::BossCommand() {}

BossCommand::~BossCommand(){}

int BossCommand::acceptOptions(int argc, char** argv) {
  // loop over input
  for (int i=0; i<argc; i++) {
    // look for a defined option
    Options_iterator oi = opt_.find(argv[i]);
    if (oi==opt_.end())
      return -1;
    // if the following string is also an option 
    // set the value for the current to TRUE
    // otherway assign to this option the value of the following string
    if ((i+1)<argc && opt_.find(argv[i+1])==opt_.end())
      oi->second=argv[++i];
    else
      oi->second="TRUE";
  }
  return 0;
}