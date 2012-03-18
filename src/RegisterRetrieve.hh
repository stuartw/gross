#ifndef REGISTERRETRIEVE_H
#define REGISTERRETRIEVE_H
#include "Log.hh"

#include "Command.hh"

class RegisterRetrieve : public Command {
public:
  RegisterRetrieve();
  ~RegisterRetrieve();
  int execute();
  void printUsage() const;
};



#endif
