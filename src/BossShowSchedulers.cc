// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossShowSchedulers.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <iostream>
#include "BossShowSchedulers.h"

#include "BossDatabase.h"

using namespace std;

BossShowSchedulers::BossShowSchedulers() : BossCommand() {
}

BossShowSchedulers::~BossShowSchedulers() {}

void BossShowSchedulers::printUsage() const
{
  cout << "Usage:" << endl
       << "boss ShowSchedulers " << endl;
  cout
       << endl;
}

int BossShowSchedulers::execute() {
  //  for (Options_const_iterator i=opt_.begin();i!=opt_.end();i++)
  //    cout << i->first << "=" << i->second << endl;

  BossDatabase db("standard");
  vector<string> sched = db.schedulers();
  for (vector<string>::const_iterator i=sched.begin(); i!=sched.end();i++) 
    cout << (*i) << endl;

  return 0;
}
