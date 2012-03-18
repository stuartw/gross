// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    boss.cpp
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include "BossCommandInterpreter.h"

int main(int argc, char** argv)
{
  BossCommandInterpreter commandInterpreter;
  // check that a command has been passed
  if (argc < 2) {
    commandInterpreter.printUsage();
    return -1;
  }
  // execute the command
  return commandInterpreter.acceptCommand(argc-1,&(argv[1]));
}
