// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossRegisterJob.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <iostream>
#include "BossRegisterJob.h"
#include "BossDatabase.h"
#include "BossOperatingSystem.h"
#include "BossJob.h"
#include <fstream>

using namespace std;

BossRegisterJob::BossRegisterJob() : BossCommand() {
  opt_["-name"] = "NULL"; 
  opt_["-schema"] = "NULL"; 
  opt_["-pre"] = "NULL"; 
  opt_["-post"] = "NULL"; 
  opt_["-runtime"] = "NULL";
  opt_["-comment"] = ""; 
}

BossRegisterJob::~BossRegisterJob() {}

void BossRegisterJob::printUsage() const
{
  cout << "Usage:" << endl
       << "boss RegisterJob " << endl;
  cout << "          -name <job name> " << endl
       << "          -schema <schema description file> " << endl
       << "          -pre <pre-process script>|SKIP " << endl
       << "          -post <post-process script>|SKIP " << endl
       << "          -runtime <runtime-process script>|SKIP " << endl
       << "          -comment <description> " << endl
       << endl;
}

int BossRegisterJob::execute() {
  //  for (Options_const_iterator i=opt_.begin();i!=opt_.end();i++)
  //    cout << i->first << "=" << i->second << endl;

  BossOperatingSystem* sys=BossOperatingSystem::instance();
  BossDatabase db("super");

  // check if the job-type exist
  if ( !(db.existJobType(opt_["-name"])) ) {
    // check if the files exist
    if ( !sys->fileExist(opt_["-schema"]) ) {
      cout << "Schema file not exist" << endl;
      return -5;
    }
  } else {
    string answer;
    cout << "WARNING, job type already registered" << endl;
    cout << "Do you want to continue ? ";
    do {
      cout << "y/n ? ";
      cin >> answer;
    } while ( answer != "y" && answer != "n" );
    if ( answer == "n" )
      return -4;
    if ( opt_["-schema"] != "NULL" ) {
      if ( !sys->fileExist(opt_["-schema"]) ) {
	cout << "Schema file not exist" << endl;
	return -5;
      }
      string answer2;
      cout << "Redefinition will cause LOST OF ALL DATA" << endl;
      cout << "Do you REALLY want to continue ? ";
      do {
	cout << "y/n ? ";
	cin >> answer2;
      } while ( answer2 != "y" && answer2 != "n" );
      if ( answer2 == "n" )
	return -4;
    }
  }
  if ( opt_["-pre"]!= "NULL" && 
       opt_["-pre"]!= "SKIP" && 
       !sys->fileExist(opt_["-pre"]) ) {
    cout << "Pre-process script not exist" << endl;
    return -6;
  }
  if ( opt_["-post"]!= "NULL" && 
       opt_["-post"]!= "SKIP" && 
       !sys->fileExist(opt_["-post"]) ) {
    cout << "Post-process script not exist" << endl;
    return -7;
  }
  if ( opt_["-runtime"]!= "NULL" && 
       opt_["-runtime"]!= "SKIP" && 
       !sys->fileExist(opt_["-runtime"]) ) {
    cout << "Runtime-process script not exist" << endl;
    return -8;
  }
  // update the database
  db.registerJob(opt_["-name"],opt_["-schema"],opt_["-comment"],
		  opt_["-pre"],opt_["-runtime"],opt_["-post"] );
  return 0;
}
