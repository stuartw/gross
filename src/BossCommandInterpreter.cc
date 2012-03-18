// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossCommandInterpreter.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <iostream>
#include "BossCommandInterpreter.h"

#include "BossCommand.h"

using namespace std;

BossCommandInterpreter::BossCommandInterpreter() {
  // DEBUG
  // cout << "    > Batch Object Submission System v 2.0 <     " << endl << endl;
  // EBND
}

BossCommandInterpreter::~BossCommandInterpreter() {

}

void BossCommandInterpreter::printUsage() const { 
  bcc_.printUsage(); 
}

// Return:
// -1 : no input 
// -2 : wrong command
// -3 : syntax error
// other : return code by the command
int BossCommandInterpreter::acceptCommand(int argc, char** argv) const {
  // check that a command has been passed
  if (argc < 1) {
    printUsage();
    return -1;
  }
  // check that the command is known
  BossCommand* c = bcc_.command(argv[0]);
  if(c==0) {
    printUsage();
    return -2;
  }
  // check that the options are compatible with command
  if(c->acceptOptions(argc-1,&(argv[1]))!=0) {
    c->printUsage();
    return -3;
  }
  int err;
  // execute the command
  if ( (err = c->execute()) )
    c->printUsage();
  
  return err;
}


