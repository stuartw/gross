// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossRecoverJob.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_RECOVER_JOB_H
#define BOSS_RECOVER_JOB_H

#include "BossCommand.h"
#include <string>

class BossJob;

class BossDatabase;

// boss RecoverJob -jobid <ID> [-noprompt]   

class BossRecoverJob : public BossCommand {
private:
  int checkRunning(BossJob*, BossDatabase*);
  int prompt(int);
  std::string redirectDir(std::string, std::string );
public:
  BossRecoverJob();
  virtual ~BossRecoverJob();
  virtual int execute();
  virtual void printUsage() const ;
};

#endif
