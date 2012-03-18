// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossQuery.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_QUERY_H
#define BOSS_QUERY_H

#include "BossCommand.h"
#include "BossJob.h"
#include <string>

class BossDatabase;
class BossScheduler;

class BossQuery : public BossCommand {
private:
  int printJobHead(BossJob*, std::string);
  int printJob(BossJob*, std::string, std::string, BossDatabase*);
  std::string getState(BossJob*, BossScheduler&);
public:
  BossQuery();
  virtual ~BossQuery();
  virtual int execute();
  virtual void printUsage() const ;
};

#endif
