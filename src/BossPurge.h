// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossQuery.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_PURGE_H
#define BOSS_PURGE_H

#include "BossCommand.h"
#include "BossJob.h"
#include <string>

// boss purge -before <DATE> [-noprompt]     

class BossPurge : public BossCommand {
private:
  int lessDate(int m1,int d1,int y1,int m2,int d2,int y2);
  int chkDate(std::string date, int& mm, int& dd, int& yy);
  int prompt();
public:
  BossPurge();
  virtual ~BossPurge();
  virtual int execute();
  virtual void printUsage() const ;
};

#endif
