// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossKernel.cc
// Authors: Claudio Grandi (INFN BO)
// Date:    20/07/2004
// Note:
// /////////////////////////////////////////////////////////////////////

#include "BossKernel.h"

#include "BossOperatingSystem.h"
#include "BossConfiguration.h"
#include "BossJob.h"

using namespace Boss;

// parses the submission instructions (possibly in a classad file)
// creates an instance for the job in the BOSS DB
// prepares the BossArchive file and the classad file to be used at submission
BossJob* Boss::declareJob(CAL::ClassAdLite& clad, BossDatabase& db) {
  //                            contains the submission details

  BossOperatingSystem* sys=BossOperatingSystem::instance();

  CAL::ClassAdLite userClad; // optional user classad
  CAL::ClassAdLite setAttr;  // optional user-defined attributes

  // if the -classad option has been used, parse the file
  if ( clad["-classad"] != "NULL" ){
    if ( !sys->fileExist(clad["-classad"]) ) {
      std::cerr << "ClassAd file doesn't exist" << std::endl;
      return 0;
    }
    // DEBUG
    // std::cout << "Before parsing" << std::endl;
    // CAL::write(clad,std::cout);
    // END DEBUG
    CAL::readFromFile(userClad,clad["-classad"]);
    parseSubmissionClassAd(clad,userClad,setAttr);
    // DEBUG
    // std::cout << "After parsing" << std::endl;
    // CAL::write(clad,std::cout);
    //CAL::write(userClad,std::cout);
    //CAL::write(setAttr,std::cout);    
    // END DEBUG    
  }
  
  // check if the job_type exists
  std::vector<std::string> types = CAL::getList(clad["-jobtype"]);
  std::vector<std::string>::const_iterator ti;
  for(ti=types.begin(); ti<types.end(); ti++) {
    if ( *ti != "stdjob" ) {  
      if ( !db.existJobType(*ti) ) {
	std::cerr << "Job type not known: " << (*ti) << std::endl;
	return 0;
      }
    }
  }
  
  // check the files
  if ( clad["-executable"]=="NULL" || 
       !sys->fileExist(clad["-executable"]) ) {
    std::cerr << "Executable doesn't exist: " << clad["-executable"] << std::endl;
    return 0;
  }
  if ( !sys->fileExist(clad["-stdin"]) ) {
    std::cerr << "Stdin doesn't exist: " << clad["-stdin"] << std::endl;
    return 0;
  }
  
  // Create new job with basic info
  
  BossJob* jobH = new BossJob();
  jobH->setUpdator(&db);
  jobH->setBasicInfo(clad["-jobtype"],
		     clad["-executable"],clad["-args"],
		     clad["-stdin"],clad["-stdout"],
		     clad["-stderr"], clad["-log"]);
  
  // Set the submission variables
  jobH->setSubInfo(sys->getHostName(),
		   sys->getCurrentDir(),
		   sys->getUserName());
  
  // Set the specific part
  for(ti=types.begin(); ti<types.end(); ti++) {
    if ( *ti != "stdjob" ) {
      std::string schema = db.findSchema(*ti);
      std::stringstream sch;
      sch << schema;
      jobH->setSchema(*ti,sch);
      //    std::cout << "BossSubmit: schema set in new job: " << schema << std::endl;
    }
  }
  
  // alloc a new job into the database
  if ( !db.insertJob(jobH) ) {
    std::cerr << "Error storing job in the Database" << std::endl;
    delete jobH;
    return 0;
  }
  int id = jobH->getId();
  std::string strid = sys->convert2string(id);
  if ( id < 0 ) {
    std::cerr << "Error assigning ID to the job" << std::endl;
    delete jobH;
    return 0;
  }
  
  // create a file with ClassAd to be used by the submit script
  std::string ocladfname = std::string("./BossClassAdFile_") + strid;
  if ( userClad.size()>0 ) 
    CAL::writeToFile(userClad,ocladfname);
  
  // If requested add the user defined attributes to the specific job type  
  if ( setAttr.size()>0 ) {
    std::string defaultTable = "";
    std::vector<std::string> types = jobH->getJobTypes();
    if ( types.size()==1 )
      defaultTable = types[0];
    CAL::ClassAdLite::const_iterator it;
    for (it=setAttr.begin(); it != setAttr.end(); it++) {
      // debug
      // std::cout << it->first << " = \"" << it->second << "\";" << std::endl;
      // end debug
      std::vector<std::string> tmp = sys->splitString(it->first,':');
      // debug
      // std::cout << tmp[0] << " " << tmp[1] << " " << it->second << std::endl;
      // end debug
      std::string table = "";
      std::string name = "";
      if        ( tmp.size()==2 ) {                           // Fully specified attribute
	table = tmp[0];
	name = tmp[1];
      } else if ( tmp.size()==1 && defaultTable.size() >0 ) { // single job type
	table = defaultTable;
	name = it->first;
      } else if ( tmp.size()==1 ) {                           // ambiguous
	std::cerr << "Cannot determine to which table " << tmp[0] << " belongs. Ingored!" << std::endl;
	continue;
      } else {                                                // invalid
	std::cerr << "Invalid attribute setting. Ignored." << std::endl;
	continue;
      }
      std::string value = it->second;
      jobH->setData(BossUpdateElement(id,table,name,value),true);
    }
  }

  BossConfiguration* config=BossConfiguration::instance();

  // create a dirctory with files to be included in archive
  std::string archdir = config->boss_tmp_dir()+"/BossJobTmp_" + strid;
  if ( sys->dirExist(archdir) ) {
    archdir+="_";
    int index=1;
    while ( sys->dirExist(archdir+sys->convert2string(index)) ) {
      index++;
    }
    archdir+=sys->convert2string(index);
  }
  if ( sys->makeDir(archdir) != 0 ) {
    std::cerr << "Cannot make archive directory";
    delete jobH; 
    return 0;
  }
  // Create the files to recreate the job at runtime (avoid query to DB)
  std::vector<std::string> files;
  if ( sys->fileCopy(config->boss_config_file(),archdir) == 0 )
    files.push_back(sys->basename(config->boss_config_file()));
  else {
    std::cerr << "Boss configuration file not found!" << std::endl;
    delete jobH;  
    return 0;
  }
  std::string jobgenname = std::string("BossGeneralInfo_") + strid;
  BossUpdateSet jobinfo;
  jobH->generalData2UpdateSet(jobinfo);
  // Remove the submission variables (not set yet...)
  jobinfo.remove(jobH->getId(),"JOB","SID");
  jobinfo.remove(jobH->getId(),"JOB","T_SUB");
  // Dump on file
  jobinfo.dump(jobgenname);
  if ( sys->fileCopy(jobgenname,archdir) == 0 ) {
    files.push_back(jobgenname);
    sys->fileRemove(jobgenname);
  }
  else {
    std::cerr << "Boss general information file not found!" << std::endl;
    delete jobH; 
    return 0;
  }
  int intErr = 0;
  // get the needed filter files from the DB
  for(ti=types.begin(); ti<types.end(); ti++) {
    if ( *ti != "stdjob" ) {  
      std::string schemafile    = archdir + "/BossSchema_" +(*ti)+"_"+strid;
      std::string prefilter     = archdir + "/BossPre_"    +(*ti)+"_"+strid;
      std::string runtimefilter = archdir + "/BossRuntime_"+(*ti)+"_"+strid;
      std::string postfilter    = archdir + "/BossPost_"   +(*ti)+"_"+strid;
      int err = -999;
      err = db.getPreProcess(*ti, prefilter);
      if ( err == 0 )
	files.push_back(sys->basename(prefilter));
      else if ( err <0 ) {
	std::cerr << "Unable to get a copy of the pre process script for " 
	     << (*ti) << ". Abort." << std::endl;
	intErr = -id;
	break;
      }
      err = db.getRuntimeProcess(*ti, runtimefilter);
      if ( err == 0 )
	files.push_back(sys->basename(runtimefilter));
      else if ( err <0 ) {
	std::cerr << "Unable to get a copy of the runtime process script for " 
	     << (*ti) << ". Abort." << std::endl;
	intErr = -id;
	break;
      }
      err = db.getPostProcess(*ti, postfilter);
      if ( err == 0 )
	files.push_back(sys->basename(postfilter));
      else if ( err <0 ) {
	std::cerr << "Unable to get a copy of the post process script for " 
	     << (*ti) << ". Abort." << std::endl;
	intErr = -id;
	break;
      }
      std::string buff = db.findSchema(*ti);
      if ( !buff.empty()) {
	std::ofstream sch(schemafile.c_str());
	if ( sch ) {
	  sch << buff;
	  sch.close();
	  files.push_back(sys->basename(schemafile));
	} else {
	  std::cerr << "Unable to write the schema for " 
	       << (*ti) << ". Abort." << std::endl;
	  intErr = -id;
	  break;
	}
      } else {
	std::cerr << "Unable to get a copy of the schema for " 
	     << (*ti) << ". Abort." << std::endl;
	intErr = -id;
	break;
      }
    }
  }

  if (intErr == 0 ) {
    // prepare the archive
    std::string currdir = sys->getCurrentDir();
    sys->changeDir(archdir);
    int tarerr = sys->tar("cz",currdir+"/BossArchive_"+strid+".tgz",files);
    sys->changeDir(currdir);
    // DEBUG
    //std::vector<std::string>dummy;
    //sys->tar("tz","BossArchive_"+strid+".tgz",dummy);
    //ENDDEBUG
    if (tarerr) {
      std::cerr << "Error creating archive" << std::endl;
      intErr = -id;
    }
  }

  // Remove archive directory
  if ( sys->dirExist(archdir) ) 
    sys->dirRemove(archdir);

  // If something went wrong delete also the classad and the archive (if any).
  if ( intErr != 0 ) {
    if ( sys->fileExist("BossArchive_"+strid+".tgz") ) 
      sys->fileRemove("BossArchive_"+strid+".tgz");
    if ( sys->fileExist(ocladfname) ) 
      sys->fileRemove(ocladfname);
    delete jobH; 
    return 0;
  }

  // done!
  return jobH;
  
}

// parses userClad and pops the attributes used by boss
// adds the attributes needed by boss to subClad
// adds the user-defined attributes (BossAttr key) to setAttr
void Boss::parseSubmissionClassAd(CAL::ClassAdLite& subClad,
			    CAL::ClassAdLite& userClad, 
			    CAL::ClassAdLite& setAttr) {
  // Support sintax for different schedulers
  // The last instance of the ClAdLookup call overwrites previous ones
  // Support Condor scheduler sintax
  CAL::pop(userClad,"Executable",subClad["-executable"]);
  CAL::pop(userClad,"Arguments",subClad["-args"]);
  CAL::pop(userClad,"input",subClad["-stdin"]);
  CAL::pop(userClad,"output",subClad["-stdout"]);
  CAL::pop(userClad,"error",subClad["-stderr"]);
  // Support EDG scheduler sintax (most c.a. are the same of condor)
  CAL::pop(userClad,"StdInput",subClad["-stdin"]);
  CAL::pop(userClad,"StdOutput",subClad["-stdout"]);
  CAL::pop(userClad,"StdError",subClad["-stderr"]);
  // Support native BOSS sintax (overrides previous settings)
  CAL::pop(userClad,"jobtype",subClad["-jobtype"]);
  CAL::pop(userClad,"executable",subClad["-executable"]);
  CAL::pop(userClad,"args",subClad["-args"]);
  CAL::pop(userClad,"stdin",subClad["-stdin"]);
  CAL::pop(userClad,"stdout",subClad["-stdout"]);
  CAL::pop(userClad,"stderr",subClad["-stderr"]);
  CAL::pop(userClad,"log",subClad["-log"]);
  // remove double quotes from attributes
  CAL::removeOuterQuotes(subClad["-jobtype"]);
  CAL::removeOuterQuotes(subClad["-executable"]);
  CAL::removeOuterQuotes(subClad["-args"]);
  CAL::removeOuterQuotes(subClad["-stdin"]);
  CAL::removeOuterQuotes(subClad["-stdout"]);
  CAL::removeOuterQuotes(subClad["-stderr"]);
  CAL::removeOuterQuotes(subClad["-log"]); 
  // Check if some attribute have to be set at submission time
  std::string setAttrStr = "";
  CAL::pop(userClad,"BossAttr",setAttrStr);
  CAL::read(setAttr,setAttrStr);
}
