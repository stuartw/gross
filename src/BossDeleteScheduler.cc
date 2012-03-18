// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossDeleteScheduler.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <iostream>
#include "BossDeleteScheduler.h"
#include "BossJob.h"
#include "BossDatabase.h"
#include "BossScheduler.h"

using namespace std;

BossDeleteScheduler::BossDeleteScheduler() : BossCommand() {
  opt_["-name"] = ""; 
  opt_["-noprompt"] = "FALSE"; 
}

BossDeleteScheduler::~BossDeleteScheduler() {}

void BossDeleteScheduler::printUsage() const
{
  cout << "Usage:" << endl
       << "boss delete " << endl;
  cout << "           -name <name> " << endl
       << "           -noprompt " << endl
       << endl;
}

int BossDeleteScheduler::execute() {
  //  for (Options_const_iterator i=opt_.begin();i!=opt_.end();i++)
  //    cout << i->first << "=" << i->second << endl;
  
  BossDatabase db("super");
  string name = opt_["-name"];

  // check if the Scheduler exists
  if ( !db.existSchType(name) ) {
    cout << "Scheduler " << name << " not found" << endl;
    return -3;
  }

  if ( opt_["-noprompt"] == "FALSE" && !prompt() ) {
    return -4;
  }

  if ( db.deleteScheduler(name) != 0 ) {
    cout << "an error occured while removing scheduler" << endl;
  }

  return 0;
}

// 1 iff the user answer y
// 0 otherwise
int BossDeleteScheduler::prompt() {

  string answer;
  cout << "WARNING the scheduler will be removed" << endl;
  cout << "Do you want to continue ? ";
  do {
    cout << "y/n ? ";
    cin >> answer;
    cout << endl;
  } while ( answer != "y" && answer != "n" );

  if ( answer == "y" ) 
    return 1;
  else
    return 0;
}
