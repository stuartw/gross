#ifndef PREPARE_H
#define PREPARE_H
#include "Log.hh"

#include "Command.hh"

class Prepare : public Command {
public:
  Prepare();
  ~Prepare();
  int execute();
  void printUsage() const;
  string workingDir();
  int taskId() const { return taskId_; };
private:
  int taskId_;
};



#endif
