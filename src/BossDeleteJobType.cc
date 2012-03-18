// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossQuery.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <iostream>
#include "BossDeleteJobType.h"
#include "BossJob.h"
#include "BossDatabase.h"
#include "BossScheduler.h"

using namespace std;

BossDeleteJobType::BossDeleteJobType() : BossCommand() {
  opt_["-name"] = ""; 
  opt_["-noprompt"] = "FALSE"; 
}

BossDeleteJobType::~BossDeleteJobType() {}

void BossDeleteJobType::printUsage() const
{
  cout << "Usage:" << endl
       << "boss delete " << endl;
  cout << "           -name <name> " << endl
       << "           -noprompt " << endl
       << endl;
}

int BossDeleteJobType::execute() {
  //  for (Options_const_iterator i=opt_.begin();i!=opt_.end();i++)
  //    cout << i->first << "=" << i->second << endl;
  
  BossDatabase db("super");
  string name = opt_["-name"];

  // check if the jobtype exists
  if ( !db.existJobType(name) ) {
    cout << "Job type " << name << " not found" << endl;
    return -3;
  }

  if ( opt_["-noprompt"] == "FALSE" && !prompt() ) {
    return -4;
  }

  if ( db.deleteJobType(name) != 0 ) {
    cout << "an error occured while removing job type" << endl;
  }

  return 0;
}

// 1 iff the user answer y
// 0 otherwise
int BossDeleteJobType::prompt() {

  string answer;
  cout << "WARNING the job type will be removed and" << endl;
  cout << "the specific data for all jobs of this type will be lost." << endl;
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
