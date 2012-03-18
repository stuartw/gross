#include <iostream>
#include "CommandInterpreter.hh"

CommandInterpreter::CommandInterpreter() {
  // DEBUG
  // cout << "    > Batch Object Submission System v 2.0 <     " << endl << endl;
  // EBND
}

CommandInterpreter::~CommandInterpreter() {

}

void CommandInterpreter::printUsage() const { 
  bcc_.printUsage(); 
}

// Return:
// -1 : no input 
// -2 : wrong command
// -3 : syntax error
// other : return code by the command
int CommandInterpreter::acceptCommand(int argc, char** argv) const {
  // check that a command has been passed
  if (argc < 1) {
    printUsage();
    return -1;
  }
  // check that the command is known
  Command* c = bcc_.command(argv[0]);
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


