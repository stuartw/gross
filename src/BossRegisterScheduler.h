// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossRegisterScheduler.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_REGISTERSCHEDULER_H
#define BOSS_REGISTERSCHEDULER_H

#include "BossCommand.h"

class BossRegisterScheduler : public BossCommand {
public:
  BossRegisterScheduler();
  virtual ~BossRegisterScheduler();
  virtual int execute();
  virtual void printUsage() const ;
};

#endif
