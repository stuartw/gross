// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossRegisterJob.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_REGISTERJOB_H
#define BOSS_REGISTERJOB_H

#include "BossCommand.h"

class BossRegisterJob : public BossCommand {
public:
  BossRegisterJob();
  virtual ~BossRegisterJob();
  virtual int execute();
  virtual void printUsage() const ;
};

#endif
