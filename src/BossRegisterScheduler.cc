// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossRegisterScheduler.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <iostream>
#include "BossRegisterScheduler.h"
#include "BossDatabase.h"
#include "OperatingSystem.h"

using namespace std;

BossRegisterScheduler::BossRegisterScheduler() : BossCommand() {
  opt_["-name"] = "NULL"; 
  opt_["-submit"] = "NULL"; 
  opt_["-kill"] = "NULL"; 
  opt_["-query"] = "NULL"; 
  opt_["-copy"] = "NONE"; 
  opt_["-topwdir"] = ""; 
  opt_["-default"] = "FALSE"; 
}

BossRegisterScheduler::~BossRegisterScheduler() {}

void BossRegisterScheduler::printUsage() const
{
  cout << "Usage:" << endl
       << "boss RegisterScheduler " << endl;
  cout << "          -name <scheduler name> " << endl
       << "          -submit <submit script> " << endl
       << "          -kill <kill script> " << endl
       << "          -query <query script> " << endl
       << "          -copy <copy command> " << endl
       << "          -topwdir <top working dir on computing hosts> " << endl
       << "          -default " << endl
       << endl;
}

int BossRegisterScheduler::execute() {
  //  for (Options_const_iterator i=opt_.begin();i!=opt_.end();i++)
  //    cout << i->first << "=" << i->second << endl;

  BossDatabase db("super");

  // check if the scheduler exists
  if ( !db.existSchType(opt_["-name"]) ) {
    // check if exists the files
    if ( !OSUtils::fileExist(opt_["-submit"]) ) {
      cout << "Submit file does not exist" << endl;
      return -5;
    }
    if ( !OSUtils::fileExist(opt_["-kill"]) ) {
      cout << "Kill file does not exist" << endl;
      return -6;
    }
    if ( !OSUtils::fileExist(opt_["-query"]) ) {
      cout << "Query file does not exist" << endl;
      return -7;
    }
  } else {
    string answer;
    cout << "WARNING, scheduler already registered" << endl;
    cout << "Do you want to overwrite it ? ";
    do {
      cout << "y/n ? ";
      cin >> answer;
      cout << endl;
    } while ( answer != "y" && answer != "n" );
    if ( answer == "n" )
      return -4;
  }
    
  // update the database
  db.registerScheduler(opt_["-name"],opt_["-default"],opt_["-topwdir"],opt_["-submit"],opt_["-kill"],opt_["-query"],opt_["-copy"]);

  return 0;
}









