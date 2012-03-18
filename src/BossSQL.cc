// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossSQL.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <iostream>
#include <ctime>
#include "BossSQL.h"
#include "BossJob.h"
#include "BossDatabase.h"
 
using namespace std;

BossSQL::BossSQL() : BossCommand() {
  opt_["-query"] = ""; 
}

BossSQL::~BossSQL() {}

void BossSQL::printUsage() const
{
  cout << "Usage:" << endl
       << "boss SQL " << endl;
  cout << "           -query SQLquery " << endl
       << endl;
}

int BossSQL::execute() {
  //  for (Options_const_iterator i=opt_.begin();i!=opt_.end();i++)
  //    cout << i->first << "=" << i->second << endl;
  
  BossDatabase db("standard");  

  if ( db.SQLquery(opt_["-query"],cout) != 0 ) {
    cout << "Your query is not correct !" << endl;
  }

  return 0;
}









