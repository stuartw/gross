// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossDeleteScheduler.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_DELETE_JOB_TYPE_H
#define BOSS_DELETE_JOB_TYPE_H

#include "BossCommand.h"

class BossDatabase;

class BossDeleteJobType : public BossCommand {
private:
  int prompt();
public:
  BossDeleteJobType();
  virtual ~BossDeleteJobType();
  virtual int execute();
  virtual void printUsage() const ;
};

#endif
