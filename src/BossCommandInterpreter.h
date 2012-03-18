// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossCommandInterpreter.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_COMMAND_INTERPRETER_H
#define BOSS_COMMAND_INTERPRETER_H

#include "BossCommandContainer.h"

class BossCommandInterpreter {
private:
  BossCommandContainer bcc_;
public:
  BossCommandInterpreter();
  ~BossCommandInterpreter();
  void printUsage() const;
  int acceptCommand(int,char**) const;
};

#endif
