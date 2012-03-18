// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossDeclare.h
// Authors: Claudio Grandi (INFN BO)
// Date:    12/07/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_DECLARE_H
#define BOSS_DECLARE_H

#include "BossCommand.h"

#include "ClassAdLite.h"

class BossDeclare : public BossCommand {
public:
  BossDeclare();
  virtual ~BossDeclare();
  virtual int execute();
  virtual void printUsage() const ;
private:
  void parseClassAd(CAL::ClassAdLite&, CAL::ClassAdLite&);
};
#endif

