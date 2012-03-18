// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossShowSchedulers.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_SHOW_SCHEDULERS_H
#define BOSS_SHOW_SCHEDULERS_H

#include "BossCommand.h"

class BossShowSchedulers : public BossCommand {
public:
  BossShowSchedulers();
  virtual ~BossShowSchedulers();
  virtual int execute();
  virtual void printUsage() const ;
};

#endif
