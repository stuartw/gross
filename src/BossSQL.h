// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossSQL.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_SQL_H
#define BOSS_SQL_H

#include "BossCommand.h"
#include "BossJob.h"

// boss SQL -query SQLquery   

class BossSQL : public BossCommand {


public:
  BossSQL();
  virtual ~BossSQL();
  virtual int execute();
  virtual void printUsage() const ;
};

#endif
