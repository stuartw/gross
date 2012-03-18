// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossKernel.cc
// Authors: Claudio Grandi (INFN BO)
// Date:    20/07/2004
// Note:
// /////////////////////////////////////////////////////////////////////

#include "BossKernel.h"

#include "OperatingSystem.h"
#include "BossConfiguration.h"
#include "BossJob.h"

using namespace Boss;

// parses the submission instructions (possibly in a classad file)
// creates an instance for the job in the BOSS DB
// prepares the BossArchive file and the classad file to be used at submission
BossJob* Boss::declareJob(CAL::ClassAdLite& clad, BossDatabase& db) {
  //                            contains the submission details

  CAL::ClassAdLite userClad; // optional user classad
  CAL::ClassAdLite setAttr;  // optional user-defined attributes

  // if the -classad option has been used, parse the file
  if ( clad["-classad"] != "NULL" ){
    if ( !OSUtils::fileExist(clad["-classad"]) ) {
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
  std::vector<std::string>::iterator ti1;
  for(ti1=types.begin(); ti1<types.end(); ti1++)
    CAL::removeOuterQuotes(*ti1);
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
       !OSUtils::fileExist(clad["-executable"]) ) {
    std::cerr << "Executable doesn't exist: " << clad["-executable"] << std::endl;
    return 0;
  }
  if ( !OSUtils::fileExist(clad["-stdin"]) ) {
    std::cerr << "Stdin doesn't exist: " << clad["-stdin"] << std::endl;
    return 0;
  }
  
  // Create new job with basic info
  
  BossJob* jobH = new BossJob();
  jobH->setUpdator(&db);
  jobH->setBasicInfo(clad["-jobtype"],
		     clad["-executable"],clad["-args"],
		     clad["-stdin"],clad["-stdout"],
		     clad["-stderr"], 
		     clad["-infiles"], clad["-outfiles"],
		     clad["-log"]);
  
  // Set the submission variables
  jobH->setSubInfo(OSUtils::getHostName(),
		   OSUtils::getCurrentDir(),
		   OSUtils::getUserName());
  
  // Set the schema for the specific part
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
  std::string strid = OSUtils::convert2string(id);
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
      std::vector<std::string> tmp = OSUtils::splitString(it->first,'.');
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
      // DEBUG
      // std::cout << "Setting private attribute " << table << ":" << name << "=" << value << std::endl;
      // END DEBUG
      jobH->setData(BossUpdateElement(id,table,name,value),true);
    }
  }

  BossConfiguration* config=BossConfiguration::instance();

  // create a dirctory with files to be included in archive
  std::string archdir = config->boss_tmp_dir()+"/BossJobTmp_" + strid;
  if ( OSUtils::dirExist(archdir) ) {
    archdir+="_";
    int index=1;
    while ( OSUtils::dirExist(archdir+OSUtils::convert2string(index)) ) {
      index++;
    }
    archdir+=OSUtils::convert2string(index);
  }
  if ( OSUtils::makeDir(archdir) != 0 ) {
    std::cerr << "Cannot make archive directory";
    delete jobH; 
    return 0;
  }
  // Create the files to recreate the job at runtime (avoid query to DB)
  std::vector<std::string> files;
  // Add dbUpdator to archive
  // assume first it is in the current directory
  std::string updatorFile = "./dbUpdator";
  if ( !OSUtils::fileExist(updatorFile) )
    // try to see if it is in the path
    updatorFile = OSUtils::which("dbUpdator");
  // copy to the archive dir
  if ( OSUtils::fileExist(updatorFile) ) {
    if ( OSUtils::fileCopy(updatorFile,archdir) == 0 )
      files.push_back(OSUtils::basename(updatorFile));
    else
      std::cerr << "Error copying updator file to archive. "
	   << "Update has to be done manually" << std::endl;      
  } else {
    std::cerr << "Database updator file not found. Update has to be done manually" 
	 << std::endl;
  }
  // add the BOSS configuration file to the archive
  if ( OSUtils::fileCopy(config->boss_config_file(),archdir) == 0 )
    files.push_back(OSUtils::basename(config->boss_config_file()));
  else {
    std::cerr << "Boss configuration file not found!" << std::endl;
    delete jobH;  
    return 0;
  }
  // add the dump of the JOB row to the archive
  std::string jobgenname = std::string("BossGeneralInfo_") + strid;
  BossUpdateSet jobinfo;
  jobH->generalData2UpdateSet(jobinfo);
  // Remove the submission variables (not set yet...)
  jobinfo.remove(jobH->getId(),"JOB","SID");
  jobinfo.remove(jobH->getId(),"JOB","T_SUB");
  // Dump on file
  jobinfo.dump(jobgenname);
  if ( OSUtils::fileCopy(jobgenname,archdir) == 0 ) {
    files.push_back(jobgenname);
    OSUtils::fileRemove(jobgenname);
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
	files.push_back(OSUtils::basename(prefilter));
      else if ( err <0 ) {
	std::cerr << "Unable to get a copy of the pre process script for " 
	     << (*ti) << ". Abort." << std::endl;
	intErr = -id;
	break;
      }
      err = db.getRuntimeProcess(*ti, runtimefilter);
      if ( err == 0 )
	files.push_back(OSUtils::basename(runtimefilter));
      else if ( err <0 ) {
	std::cerr << "Unable to get a copy of the runtime process script for " 
	     << (*ti) << ". Abort." << std::endl;
	intErr = -id;
	break;
      }
      err = db.getPostProcess(*ti, postfilter);
      if ( err == 0 )
	files.push_back(OSUtils::basename(postfilter));
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
	  files.push_back(OSUtils::basename(schemafile));
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
  // Add user declared input sandbox files to archive
  std::vector<std::string> inf = jobH->getInFiles();
  std::vector<std::string>::const_iterator itf;
  for (itf=inf.begin();itf!=inf.end(); ++itf) {
    if ( OSUtils::fileCopy((*itf),archdir) == 0 )
      files.push_back(OSUtils::basename(*itf));
    else {
      std::cerr << "Unable to add file " << (*itf) << " to archive." 
		<< " Abort!" << std::endl;
      intErr = -id;
    }
  }
  
  if (intErr == 0 ) {
    // prepare the archive
    std::string currdir = OSUtils::getCurrentDir();
    OSUtils::changeDir(archdir);
    // DEBUG
    // OSUtils::shell("ls -l ");
    // ENDDEBUG
    int tarerr = OSUtils::tar("cz",currdir+"/BossArchive_"+strid+".tgz",files);
    OSUtils::changeDir(currdir);
    // DEBUG
    //std::vector<std::string>dummy;
    //OSUtils::tar("tz","BossArchive_"+strid+".tgz",dummy);
    //ENDDEBUG
    if (tarerr) {
      std::cerr << "Error creating archive" << std::endl;
      intErr = -id;
    }
  }

  // Remove archive directory
  if ( OSUtils::dirExist(archdir) ) 
    OSUtils::dirRemove(archdir);

  // If something went wrong delete also the classad and the archive (if any).
  if ( intErr != 0 ) {
    if ( OSUtils::fileExist("BossArchive_"+strid+".tgz") ) 
      OSUtils::fileRemove("BossArchive_"+strid+".tgz");
    if ( OSUtils::fileExist(ocladfname) ) 
      OSUtils::fileRemove(ocladfname);
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
  CAL::pop(userClad,"transfer_input_files",subClad["-infiles"]);
  CAL::pop(userClad,"transfer_output_files",subClad["-outfiles"]);
  std::string dummy;
  CAL::pop(userClad,"should_transfer_files",dummy);
  CAL::pop(userClad,"when_to_transfer_output",dummy);
  // Support EDG scheduler sintax (most c.a. are the same of condor)
  CAL::pop(userClad,"StdInput",subClad["-stdin"]);
  CAL::pop(userClad,"StdOutput",subClad["-stdout"]);
  CAL::pop(userClad,"StdError",subClad["-stderr"]);
  CAL::pop(userClad,"InputSandbox",subClad["-infiles"]);
  CAL::pop(userClad,"OutputSandbox",subClad["-outfiles"]);
  // Support native BOSS sintax (overrides previous settings)
  CAL::pop(userClad,"jobtype",subClad["-jobtype"]);
  CAL::pop(userClad,"executable",subClad["-executable"]);
  CAL::pop(userClad,"args",subClad["-args"]);
  CAL::pop(userClad,"stdin",subClad["-stdin"]);
  CAL::pop(userClad,"stdout",subClad["-stdout"]);
  CAL::pop(userClad,"stderr",subClad["-stderr"]);
  CAL::pop(userClad,"log",subClad["-log"]);
  CAL::pop(userClad,"infiles",subClad["-infiles"]);
  CAL::pop(userClad,"outfiles",subClad["-outfiles"]);
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
