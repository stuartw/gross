// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossKernel.h
// Authors: Claudio Grandi (INFN BO)
// Date:    20/07/2004
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_KERNEL_H
#define BOSS_KERNEL_H

#include "ClassAdLite.h"
#include "BossDatabase.h"

class BossJob;

namespace Boss {
  BossJob* declareJob(CAL::ClassAdLite&, BossDatabase&);
  void parseSubmissionClassAd(CAL::ClassAdLite&,CAL::ClassAdLite&,CAL::ClassAdLite&);
}

#endif
