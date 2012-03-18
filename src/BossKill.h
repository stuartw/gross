// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossKill.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_KILL_H
#define BOSS_KILL_H

#include "BossCommand.h"

class BossKill : public BossCommand {
public:
  BossKill();
  virtual ~BossKill();
  virtual int execute();
  virtual void printUsage() const ;
};

#endif
