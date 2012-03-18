#ifndef ONESTEP_H
#define ONESTEP_H
#include "Log.hh"

#include "Command.hh"

class Prepare;
class Submit;
class Output;

/*!
  \Command to run the Prepare and Submit modules concurently
  

*/


class OneStep : public Command {
public:
  OneStep();
  ~OneStep();
  int execute();
  void printUsage() const;
private:
  //CommandInterpreter _commandInterpreter;
  Prepare* prepare_;
  Submit* submit_;
  Output* output_;
  int taskId;
};



#endif
