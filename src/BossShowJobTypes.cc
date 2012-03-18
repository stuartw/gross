// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossShowJobTypes.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <iostream>
#include "BossShowJobTypes.h"

#include "BossDatabase.h"

using namespace std;

BossShowJobTypes::BossShowJobTypes() : BossCommand() {
}

BossShowJobTypes::~BossShowJobTypes() {}

void BossShowJobTypes::printUsage() const
{
  cout << "Usage:" << endl
       << "boss ShowJobTypes " << endl;
  cout
       << endl;
}

int BossShowJobTypes::execute() {
  //  for (Options_const_iterator i=opt_.begin();i!=opt_.end();i++)
  //    cout << i->first << "=" << i->second << endl;

  BossDatabase db("standard");
  vector<string> jt = db.jobTypes();
  for (vector<string>::const_iterator i=jt.begin(); i!=jt.end();i++) 
    cout << (*i) << endl;

  return 0;
}
