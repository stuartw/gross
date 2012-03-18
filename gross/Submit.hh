#ifndef SUBMIT_H
#define SUBMIT_H
#include "Log.hh"

#include "Command.hh"

class Submit : public Command {
public:
  Submit();
  ~Submit();
  int execute();
  void printUsage() const;
};



#endif
