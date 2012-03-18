#include "CommandInterpreter.hh"
#include "Log.hh"
#include "FileSys.hh"
#include <string>

//int Log::level_=0; //Default Logging Level
//string FileSys::oDir_="./"; //Default Output Directory

int main(int argc, char** argv)
{
  CommandInterpreter commandInterpreter;
  // check that a command has been passed
  if (argc < 2) {
    commandInterpreter.printUsage();
    return -1;
  }
  // execute the command
  return commandInterpreter.acceptCommand(argc-1,&(argv[1]));
}
