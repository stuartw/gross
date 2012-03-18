#ifndef COMMAND_INTERPRETER_H
#define COMMAND_INTERPRETER_H
#include "Log.hh"

#include "Command.hh"
#include "CommandContainer.hh"

class CommandInterpreter {
private:
  CommandContainer bcc_;
public:
  CommandInterpreter();
  ~CommandInterpreter();
  void printUsage() const;
  int acceptCommand(int,char**) const;
};

#endif
