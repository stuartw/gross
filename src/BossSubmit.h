// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossSubmit.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_SUBMIT_H
#define BOSS_SUBMIT_H

#include "BossCommand.h"

#include <vector>
#include <string>

class BossClassAd;

class BossSubmit : public BossCommand {
public:
  BossSubmit();
  virtual ~BossSubmit();
  virtual int execute();
  virtual void printUsage() const ;
private:
  void parseClassAd(BossClassAd*,std::vector< std::pair <std::string,std::string> >*);
};
#endif

