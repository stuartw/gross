// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossQuery.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_DELETE_H
#define BOSS_DELETE_H

#include "BossCommand.h"
#include "BossJob.h"

class BossDatabase;

// boss delete -jobid <ID> [-noprompt]   

class BossDelete : public BossCommand {
private:
  int checkRunning(BossJob*, BossDatabase*);
  int prompt();
public:
  BossDelete();
  virtual ~BossDelete();
  virtual int execute();
  virtual void printUsage() const ;
};

#endif
