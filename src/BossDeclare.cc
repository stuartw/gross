// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossDeclare.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <sstream>
#include <iostream>

#include "BossDeclare.h"

#include "BossDatabase.h"
#include "BossOperatingSystem.h"
#include "BossClassAd.h"
#include "BossJob.h"

using namespace std;

BossDeclare::BossDeclare() : BossCommand() {
  opt_["-jobtype"] = "stdjob"; 
  opt_["-executable"] = "NULL"; 
  opt_["-args"] = ""; 
  opt_["-stdin"] = "/dev/null"; 
  opt_["-stdout"] = "/dev/null"; 
  opt_["-stderr"] = "/dev/null"; 
  opt_["-classad"] = "NULL"; 
  opt_["-log"] = "NULL"; 
}

BossDeclare::~BossDeclare() {}

void BossDeclare::printUsage() const
{
  cerr << "Usage:" << endl
       << "boss declare " << endl
       << "            [ -jobtype <registered job type> (stdjob)" << endl
       << "              -executable <executable file to be submitted>" << endl
       << "              -args <arguments to the executable>" << endl
       << "              -stdin <standard input file>" << endl
       << "              -stdout <standard output file>" << endl
       << "              -stderr <standard error file>" << endl
       << "              -log <scheduler log file> ]" << endl
       << "            OR " << endl
       << "            [ -classad <classad file> ]" << endl
       << endl;
}

int BossDeclare::execute() {
  //  for (Options_const_iterator i=opt_.begin();i!=opt_.end();i++)
  //    cout << i->first << "=" << i->second << endl;

  BossOperatingSystem* sys=BossOperatingSystem::instance();
  BossDatabase db("super");

  // if the -classad option has been used, parse the file
  BossClassAd* clad = 0;
  if ( opt_["-classad"] != "NULL" ){
    if ( !sys->fileExist(opt_["-classad"]) ) {
      cerr << "ClassAd file doesn't exist" << endl;
      return -10;
    }
    clad = new BossClassAd(opt_["-classad"]);
    parseClassAd(clad);
  }

  // check if the job_type exists 
  vector<string> types = sys->splitString(opt_["-jobtype"],',');
  vector<string>::const_iterator ti;
  for(ti=types.begin(); ti<types.end(); ti++) {
    if ( *ti != "stdjob" ) {  
      if ( !db.existJobType(*ti) ) {
	cerr << "Job type not known: " << (*ti) << endl;
	return -3;
      }
    }
  }

  // check the files
  if ( opt_["-executable"]=="NULL" || !sys->fileExist(opt_["-executable"]) ) {
    cerr << "Executable doesn't exist: " << opt_["-executable"] << endl;
    return -5;
  }
  if ( !sys->fileExist(opt_["-stdin"]) ) {
    cerr << "Stdin doesn't exist: " << opt_["-stdin"] << endl;
    return -5;
  }
  
  // Create new job with basic info
  BossJob* jobH = new BossJob();
  jobH->setUpdator(&db);
  jobH->setBasicInfo(opt_["-jobtype"],opt_["-scheduler"],
		     opt_["-executable"],opt_["-args"],
		     opt_["-stdin"],opt_["-stdout"],
		     opt_["-stderr"], opt_["-log"]);
  
  // Set the submission variables
  jobH->setSubInfo(sys->getHostName(),
		   sys->getCurrentDir(),
		   sys->getUserName());

  // Set the specific part
  for(ti=types.begin(); ti<types.end(); ti++) {
    if ( *ti != "stdjob" ) {
      string schema = db.findSchema(*ti);
      std::stringstream sch;
      sch << schema;
      jobH->setSchema(*ti,sch);
      //    cout << "BossSubmit: schema set in new job: " << schema << endl;
    }
  }

  // alloc a new job into the database
  if ( !db.insertJob(jobH) ) {
    cerr << "Error storing job in the Database" << endl;
    return -7;
  }
  if ( jobH->getId() < 0 ) {
    cerr << "Error assigning ID to the job" << endl;
    return -8;
  }
  cout << "Job ID " << jobH->getId() << endl;

  // create a file with ClassAd to be used by the submit script
  string ocladfname = string("./BossClassAdFile_") + sys->convert2string(jobH->getId());
  if ( clad ) 
    clad->dumpClad(ocladfname);

  // delete the classad
  if ( clad ) 
    delete clad;

  // delete the job handle
  delete jobH; 

  return 0;
}

void BossDeclare::parseClassAd(BossClassAd* classad) {
  // Support sintax for different schedulers
  // The last instance of the ClAdLookup call overwrites previous ones
  // Support Condor scheduler sintax
  classad->ClAdLookup( "Executable", &(opt_["-executable"]) ); 
  classad->ClAdLookup( "Arguments" , &(opt_["-args"])       ); 
  classad->ClAdLookup( "input"     , &(opt_["-stdin"])      );
  classad->ClAdLookup( "output"    , &(opt_["-stdout"])     );
  classad->ClAdLookup( "error"     , &(opt_["-stderr"])     );
  // Support EDG scheduler sintax (most c.a. are the same of condor)
  classad->ClAdLookup( "StdInput"  , &(opt_["-stdin"])      );
  classad->ClAdLookup( "StdOutput" , &(opt_["-stdout"])      );
  classad->ClAdLookup( "StdError"  , &(opt_["-stderr"])     );
  // Support native BOSS sintax
  classad->ClAdLookup( "jobtype"   , &(opt_["-jobtype"])    );
  classad->ClAdLookup( "executable", &(opt_["-executable"]) ); 
  classad->ClAdLookup( "args"      , &(opt_["-args"])       ); 
  classad->ClAdLookup( "stdin"     , &(opt_["-stdin"])      );
  classad->ClAdLookup( "stdout"    , &(opt_["-stdout"])     );
  classad->ClAdLookup( "stderr"    , &(opt_["-stderr"])     );
  classad->ClAdLookup( "log"       , &(opt_["-log"])        );
}
