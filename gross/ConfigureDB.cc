#include "ConfigureDB.hh"

//#include "TaskFactory.hh"
//#include "Task.hh"
#include "FileSys.hh"
#include "File.hh"
#include "DbSpec.hh"
#include "LocalDb.hh"
#include <stdlib.h>
ConfigureDB::ConfigureDB() : Command() {
  opt_["-logLevel"]  = "0";
}
ConfigureDB::~ConfigureDB(){
  if (dbSpec_) delete dbSpec_;
}

void ConfigureDB::printUsage() const
{
  cerr << "Usage:" << endl
       << "gross configureDB " << endl;
  cerr
       << "Should not be called directly" << endl
       << "it is called at compile time" << endl
       << endl;
}
int ConfigureDB::execute() {

  //set verbosity
  Log::level(atoi((opt_["-logLevel"].c_str())));
	
  //Initialise Db
  if(initDb()) return EXIT_FAILURE;
  
  //create file to hold SQL commands
  File output("GrossDBConfig.sql");
  if (output.exists()) output.remove();
 
  string filename;
  char* pc = getenv("GROSS_DBSPEC");
  if (pc) filename = pc;
  else {
    pc = getenv("BOSSDIR");
    if(pc) {
      filename = pc + string("/BossConfig.clad");
    }
    else {
      cerr<<"Error : DB spec file not set and not found as environment variable GROSS_DBSPEC or BOSSDIR"<<endl;
      return EXIT_FAILURE;
    }
  }
 

  
  File myDbSpecFile(filename);
  if(!myDbSpecFile.exists()) {
    cerr<< "Error : Cannot find db spec file " << myDbSpecFile.fullHandle()<<endl;
    return EXIT_FAILURE;
  } 
  DbSpec* dbSpec_ = new DbSpec(myDbSpecFile.contents()); //Deleted in destructor
  
  //code duplication?
  //test DB exists and is connectable
  /*if(LocalDb::instance()->init(dbSpec_)) {
    cerr << "Error : Failure connecting to db");
    cerr << "Ensure BOSS db configure script has been run");
    return EXIT_FAILURE;
  } */ 
  
  //do sql commands
  vector<string> values;
  
  //Analy_Task
  values.push_back("ID INT AUTO_INCREMENT NOT NULL PRIMARY KEY");
  values.push_back("UserSpec MEDIUMBLOB NOT NULL DEFAULT \"\"");
  values.push_back("JDLRem MEDIUMBLOB NOT NULL DEFAULT \"\"");
  values.push_back("FacType VARCHAR(30) NOT NULL DEFAULT \"\"");
  if(LocalDb::instance()->tableCreate("Analy_Task", values))
    return EXIT_FAILURE;
   
  //Analy_Job
  values.clear();
  values.push_back("ID INT AUTO_INCREMENT NOT NULL PRIMARY KEY");
  values.push_back("TaskID INT");
  values.push_back("JobID INT");
  values.push_back("BossID INT NOT NULL DEFAULT 0");
  values.push_back("DataSelect VARCHAR(255)");
  values.push_back("ExecName VARCHAR(255)");
  values.push_back("StdOut VARCHAR(255)");
  values.push_back("StdErr VARCHAR(255)");
  values.push_back("Suffix VARCHAR(255)");
  values.push_back("XMLFrag MEDIUMBLOB DEFAULT \"\"");
  values.push_back("SboxDir VARCHAR(255)");
  values.push_back("MetaFile VARCHAR(255)");
  values.push_back("outDir VARCHAR(255)");
  values.push_back("runsPerJob INT");
  if(LocalDb::instance()->tableCreate("Analy_Job", values))
    return EXIT_FAILURE;
		
  //Analy_InGUIDs
  values.clear();
  values.push_back("ID INT AUTO_INCREMENT NOT NULL PRIMARY KEY");
  values.push_back("TaskID INT");
  values.push_back("JobID INT");
  values.push_back("Name VARCHAR(255)");
  if(LocalDb::instance()->tableCreate("Analy_InGUIDs", values))
    return EXIT_FAILURE;
		  
  //Analy_InMETAs
  values.clear();
  values.push_back("ID INT AUTO_INCREMENT NOT NULL PRIMARY KEY");
  values.push_back("TaskID INT");
  values.push_back("JobID INT");
  values.push_back("Name VARCHAR(255)");
  if(LocalDb::instance()->tableCreate("Analy_InMETAs", values))
    return EXIT_FAILURE;
  
  //Analy_OutGUIDs
  values.clear();
  values.push_back("ID INT AUTO_INCREMENT NOT NULL PRIMARY KEY");
  values.push_back("TaskID INT");
  values.push_back("JobID INT");
  values.push_back("Name VARCHAR(255)");
  if(LocalDb::instance()->tableCreate("Analy_OutGUIDs", values))
    return EXIT_FAILURE;
  
  //Analy_LocalIn
  values.clear();
  values.push_back("ID INT AUTO_INCREMENT NOT NULL PRIMARY KEY");
  values.push_back("TaskID INT");
  values.push_back("JobID INT");
  values.push_back("Name VARCHAR(255)");
  if(LocalDb::instance()->tableCreate("Analy_LocalIn", values))
    return EXIT_FAILURE;
	      
  //Analy_OutSandbox
  values.clear();
  values.push_back("ID INT AUTO_INCREMENT NOT NULL PRIMARY KEY");
  values.push_back("TaskID INT");
  values.push_back("JobID INT");
  values.push_back("Name VARCHAR(255)");
  if(LocalDb::instance()->tableCreate("Analy_OutSandbox", values))
    return EXIT_FAILURE;
	      
  //Analy_Wrapper
  values.clear();
  values.push_back("name VARCHAR(255) NOT NULL PRIMARY KEY");
  values.push_back("script MEDIUMBLOB NOT NULL DEFAULT \"\"");
  if(LocalDb::instance()->tableCreate("Analy_Wrapper", values))
    return EXIT_FAILURE;
	      
  //AnalyLoc_InFiles
  values.clear();
  values.push_back("ID INT AUTO_INCREMENT NOT NULL PRIMARY KEY");
  values.push_back("TaskID INT");
  values.push_back("JobID INT");
  values.push_back("Name VARCHAR(255)");
  if(LocalDb::instance()->tableCreate("AnalyLoc_InFiles", values))
    return EXIT_FAILURE;
  
  //AnalyLoc_OutFiles
  values.clear();
  values.push_back("ID INT AUTO_INCREMENT NOT NULL PRIMARY KEY");
  values.push_back("TaskID INT");
  values.push_back("JobID INT");
  values.push_back("Name VARCHAR(255)");
  if(LocalDb::instance()->tableCreate("AnalyLoc_OutFiles", values))
    return EXIT_FAILURE;
		
  return EXIT_SUCCESS;
}
