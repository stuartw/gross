// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossSubmit.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "BossSubmit.h"

#include "BossConfiguration.h"
#include "BossDatabase.h"
#include "BossOperatingSystem.h"
#include "BossClassAd.h"
#include "BossUpdateSet.h"
#include "BossScheduler.h"
#include "BossJob.h"

using namespace std;

BossSubmit::BossSubmit() : BossCommand() {
  opt_["-jobid"] = "NULL"; 
  opt_["-jobtype"] = "stdjob"; 
  opt_["-scheduler"] = "NULL"; 
  opt_["-executable"] = "NULL"; 
  opt_["-args"] = ""; 
  opt_["-stdin"] = "/dev/null"; 
  opt_["-stdout"] = "/dev/null"; 
  opt_["-stderr"] = "/dev/null"; 
  opt_["-classad"] = "NULL"; 
  opt_["-host"] = "NULL";
  opt_["-log"] = "NULL"; 
}

BossSubmit::~BossSubmit() {}

void BossSubmit::printUsage() const
{
  cerr << "Usage:" << endl
       << "boss submit " << endl;
  cerr
       << "            -scheduler <registered scheduler>" << endl
       << "            -host <hostname>" << endl
       << "            [ -jobtype <registered job type> (stdjob)" << endl
       << "              -executable <executable file to be submitted>" << endl
       << "              -args <arguments to the executable>" << endl
       << "              -stdin <standard input file>" << endl
       << "              -stdout <standard output file>" << endl
       << "              -stderr <standard error file>" << endl
       << "              -log <scheduler log file> ]" << endl
       << "            OR " << endl
       << "            [ -classad <classad file> ]" << endl
       << "            OR " << endl
       << "            [ -jobid <declared job identifier> ]" << endl
       << endl;
}

int BossSubmit::execute() {
  int ret_val = 0;
  //  for (Options_const_iterator i=opt_.begin();i!=opt_.end();i++)
  //    cout << i->first << "=" << i->second << endl;

  BossOperatingSystem* sys=BossOperatingSystem::instance();
  BossDatabase db("super");

  BossJob* jobH = 0;

  vector< pair <string,string> > setAttr;

  // check if the scheduler exists
  if ( opt_["-scheduler"]!="NULL" && !db.existSchType(opt_["-scheduler"]) ) {
    cerr << "Scheduler not known: " << opt_["-jobtype"] << endl;
    return -4;
  }
  if ( opt_["-scheduler"]=="NULL" ) {
    // get the default scheduler
    opt_["-scheduler"] = db.getDefaultSch();
    if (  opt_["-scheduler"] == "NULL" ) {
      cerr << "Default Scheduler not set!" << endl;
      return -5;
    }
  }
    
  // If the -jobid option is used, try to get the job handle from the DB
  if ( opt_["-jobid"] != "NULL" ) {
    
    jobH = db.findJob(atoi(opt_["-jobid"].c_str()));
    if ( !jobH ) {
      cout << "JobID " <<  opt_["-jobid"] << " not found" << endl;
      return -3;
    }
    
  } else {
    
    // if the -classad option has been used, parse the file
    BossClassAd* clad = 0;
    if ( opt_["-classad"] != "NULL" ){
      if ( !sys->fileExist(opt_["-classad"]) ) {
	cerr << "ClassAd file doesn't exist" << endl;
	return -10;
      }
      clad = new BossClassAd(opt_["-classad"]);
      parseClassAd(clad,&setAttr);
      // debug
      //        vector< pair<string,string> >::const_iterator it;
      //        for (it =setAttr.begin(); it != setAttr.end(); it++)
      //  	cout << it->first << " = " << it->second << endl;
      // end debug
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
    if ( opt_["-executable"]=="NULL" || 
	 !sys->fileExist(opt_["-executable"]) ) {
      cerr << "Executable doesn't exist: " << opt_["-executable"] << endl;
      return -5;
    }
    if ( !sys->fileExist(opt_["-stdin"]) ) {
      cerr << "Stdin doesn't exist: " << opt_["-stdin"] << endl;
      return -5;
    }
    
    // Create new job with basic info
    jobH = new BossJob();
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
    int id = jobH->getId();
    string strid = sys->convert2string(id);
    if ( id < 0 ) {
      cerr << "Error assigning ID to the job" << endl;
      return -8;
    }
    cout << "Job ID " << id << endl;
    
    // create a file with ClassAd to be used by the submit script
    string ocladfname = string("./BossClassAdFile_") + strid;
    if ( clad ) 
      clad->dumpClad(ocladfname);
    
    // delete the classad
    if ( clad ) 
      delete clad;
    
    // If requested add the user defined attributes to the specific job type  
    if ( setAttr.size()>0 ) {
      string defaultTable = "";
      vector<string> types = jobH->getJobTypes();
      if ( types.size()==1 )
	defaultTable = types[0];
      vector< pair<string,string> >::const_iterator it;
      for (it=setAttr.begin(); it != setAttr.end(); it++) {
	// debug
	// cout << it->first << " = \"" << it->second << "\";" << endl;
	// end debug
	vector<string> tmp = sys->splitString(it->first,':');
	// debug
	// cout << tmp[0] << " " << tmp[1] << " " << it->second << endl;
	// end debug
	string table = "";
	string name = "";
	if        ( tmp.size()==2 ) {                           // Fully specified attribute
	  table = tmp[0];
	  name = tmp[1];
	} else if ( tmp.size()==1 && defaultTable.size() >0 ) { // single job type
	  table = defaultTable;
	  name = it->first;
	} else if ( tmp.size()==1 ) {                           // ambiguous
	  cerr << "Cannot determine to which table " << tmp[0] << " belongs. Ingored!" << endl;
	  continue;
        } else {                                                // invalid
	  cerr << "Invalid attribute setting. Ignored." << endl;
	  continue;
	}
	string value = it->second;
	jobH->setData(BossUpdateElement(id,table,name,value),true);
      }
    }
    
  }

  int id = jobH->getId();
  string strid = sys->convert2string(id);
  // Create the files to recreate the job at runtime (avoid query to DB)
  vector<string> files;
  BossConfiguration* config=BossConfiguration::instance();
  if ( sys->fileCopy(config->boss_config_file(),".") == 0 )
    files.push_back(sys->basename(config->boss_config_file()));
  else {
    cerr << "Boss configuration file not found!" << endl;
    return -14;
  }
  string jobgenname = string("./BossGeneralInfo_") + strid;
  BossUpdateSet jobinfo;
  jobH->generalData2UpdateSet(jobinfo);
  // Remove the submission variables (not set yet...)
  jobinfo.remove(jobH->getId(),"JOB","SID");
  jobinfo.remove(jobH->getId(),"JOB","T_SUB");
  // Dump on file
  jobinfo.dump(jobgenname);
  files.push_back(jobgenname);
  // get the needed filter files from the DB
  vector<string> types = jobH->getJobTypes();
  vector<string>::const_iterator ti;
  for(ti=types.begin(); ti<types.end(); ti++) {
    if ( *ti != "stdjob" ) {  
      string schemafile    = string("./BossSchema_") +(*ti)+"_"+strid;
      string prefilter     = string("./BossPre_")    +(*ti)+"_"+strid;
      string runtimefilter = string("./BossRuntime_")+(*ti)+"_"+strid;
      string postfilter    = string("./BossPost_")   +(*ti)+"_"+strid;
      int err = -999;
      err = db.getPreProcess(*ti, prefilter);
      if ( err == 0 )
	files.push_back(prefilter);
      else if ( err <0 ) {
	cerr << "Unable to get a copy of the pre process script for " 
	     << (*ti) << ". Abort." << endl;
	ret_val = -7;
	break;
      }
      err = db.getRuntimeProcess(*ti, runtimefilter);
      if ( err == 0 )
	files.push_back(runtimefilter);
      else if ( err <0 ) {
	cerr << "Unable to get a copy of the runtime process script for " 
	     << (*ti) << ". Abort." << endl;
	ret_val = -8;
	break;
      }
      err = db.getPostProcess(*ti, postfilter);
      if ( err == 0 )
	files.push_back(postfilter);
      else if ( err <0 ) {
	cerr << "Unable to get a copy of the post process script for " 
	     << (*ti) << ". Abort." << endl;
	ret_val = -9;
	break;
      }
      string buff = db.findSchema(*ti);
      if ( !buff.empty()) {
	ofstream sch(schemafile.c_str());
	if ( sch ) {
	  sch << buff;
	  sch.close();
	  files.push_back(schemafile);
	} else {
	  cerr << "Unable to write the schema for " 
	       << (*ti) << ". Abort." << endl;
	  ret_val = -11;
	  break;
	}
      } else {
	cerr << "Unable to get a copy of the schema for " 
	     << (*ti) << ". Abort." << endl;
	ret_val = -10;
	break;
      }
    }
  }

  // If everything is ok, submit the job to the local scheduler
  if (ret_val == 0 ) {
    // prepare the archive
    if (sys->tar("cz","BossArchive_"+strid+".tgz",files)) {
      cerr << "Error creating archive" << endl;
      ret_val = -12;
    } else {
      BossScheduler sched(&db);
      if ( sched.submit(jobH, opt_["-host"]) != 0 ) {
	cerr << "Unable to submit job" << endl;
	ret_val = -13;
      }
    }
  }    
  // DEBUG
  // cerr << "Job status after submission:" << endl;
  // jobH->dumpGeneral(cerr);
  // jobH->dumpSpecific(cerr);
  // END DEBUG
  // Remove the temporary ClassAd file used by the submit script
  string ocladfname = string("./BossClassAdFile_") + strid;
  if ( sys->fileExist(ocladfname) ) sys->fileRemove(ocladfname);
  // delete the job handle
  delete jobH; 
  // Remove files in the archive
  vector<string>::const_iterator it;
  for (it=files.begin(); it<files.end(); it++ )
    if ( sys->fileExist(*it) ) sys->fileRemove(*it);
  // Something went wrong. Delete also the archive (if any).
  if ( ret_val != 0 ) 
    if ( sys->fileExist("BossArchive_"+strid+".tgz") ) 
      sys->fileRemove("BossArchive_"+strid+".tgz");
  return ret_val;
}

void BossSubmit::parseClassAd(BossClassAd* classad, 
			      vector< pair <string,string> >* setAttr) {
  // Support sintax for different schedulers
  // The last instance of the ClAdLookup call overwrites previous ones
  // Support Condor scheduler sintax
  classad->ClAdLookup( "Executable", &(opt_["-executable"]) ); 
  classad->ClAdLookup( "Arguments" , &(opt_["-args"])       ); 
  classad->ClAdLookup( "input"     , &(opt_["-stdin"])      );
  classad->ClAdLookup( "output"    , &(opt_["-stdout"])     );
  classad->ClAdLookup( "error"     , &(opt_["-stderr"])     );
  // N.B. ClAdExtractFromExpr doesn't remove the string from ClassAd!!!
  classad->ClAdExtractFromExpr( "Resources", "Machine", &(opt_["-host"]) );
  // Support EDG scheduler sintax (most c.a. are the same of condor)
  classad->ClAdLookup( "StdInput"  , &(opt_["-stdin"])      );
  classad->ClAdLookup( "StdOutput" , &(opt_["-stdout"])     );
  classad->ClAdLookup( "StdError"  , &(opt_["-stderr"])     );
  // Support native BOSS sintax
  classad->ClAdLookup( "jobtype"   , &(opt_["-jobtype"])    );
  classad->ClAdLookup( "scheduler" , &(opt_["-scheduler"])  );
  classad->ClAdLookup( "executable", &(opt_["-executable"]) ); 
  classad->ClAdLookup( "args"      , &(opt_["-args"])       ); 
  classad->ClAdLookup( "stdin"     , &(opt_["-stdin"])      );
  classad->ClAdLookup( "stdout"    , &(opt_["-stdout"])     );
  classad->ClAdLookup( "stderr"    , &(opt_["-stderr"])     );
  classad->ClAdLookup( "log"       , &(opt_["-log"])        );
  classad->ClAdLookup( "host"      , &(opt_["-host"])       );
  // Check if some attribute have to be set at submission time
  classad->NestedClAdLookup( "BossAttr", setAttr );
}
