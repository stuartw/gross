// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossVersion.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <iostream>

#include "BossVersion.h"

#include "BossConfiguration.h"

using namespace std;

BossVersion::BossVersion() : BossCommand() {}

BossVersion::~BossVersion() {}

int BossVersion::execute() {
  BossConfiguration* config=BossConfiguration::instance();
  cout << "BOSS Version " << config->boss_version() << endl; 
  return 0;
}
void BossVersion::printUsage() const {}
