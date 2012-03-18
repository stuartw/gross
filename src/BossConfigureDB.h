// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 1.0
// File:    BossConfigureDB.h
// Authors: Claudio Grandi (INFN BO)
// Date:    28/11/2003
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_CONFIGURE_DB_H
#define BOSS_CONFIGURE_DB_H

#include "BossCommand.h"

// boss SQL -query SQLquery   

class BossConfigureDB : public BossCommand {


public:
  BossConfigureDB();
  virtual ~BossConfigureDB();
  virtual int execute();
  virtual void printUsage() const ;
};

#endif
