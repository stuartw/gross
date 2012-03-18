// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossVersion.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_VERSION_H
#define BOSS_VERSION_H

#include "BossCommand.h"

class BossVersion : public BossCommand {
public:
  BossVersion();
  virtual ~BossVersion();
  virtual int execute();
  virtual void printUsage() const ;
};

#endif


