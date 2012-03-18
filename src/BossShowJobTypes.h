// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossShowJobTypes.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_SHOW_JOB_TYPES_H
#define BOSS_SHOW_JOB_TYPES_H

#include "BossCommand.h"

class BossShowJobTypes : public BossCommand {
public:
  BossShowJobTypes();
  virtual ~BossShowJobTypes();
  virtual int execute();
  virtual void printUsage() const ;
};

#endif
