#ifndef REGISTER_H
#define REGISTER_H
#include "Log.hh"

#include "Command.hh"

class Register : public Command {
public:
  Register();
  ~Register();
  int execute();
  void printUsage() const;
};



#endif
