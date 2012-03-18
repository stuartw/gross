#include "Command.hh"

#include "LocalDb.hh"
#include "DbSpec.hh"
#include "File.hh"

Command::Command() : dbSpec_(0){}

Command::~Command(){
  if(dbSpec_) delete dbSpec_;
}

int Command::acceptOptions(int argc, char** argv) {
  // loop over input
  for (int i=0; i<argc; i++) {
    // look for a defined option
    Options_iterator oi = opt_.find(argv[i]);
    if (oi==opt_.end())
      return -1;
    // if the following string is also an option 
    // set the value for the current to TRUE
    // otherway assign to this option the value of the following string
    if ((i+1)<argc && opt_.find(argv[i+1])==opt_.end())
      oi->second=argv[++i];
    else
      oi->second="TRUE";
  }
  return 0;
}

//Some common (useful) methods
int Command::initDb() {  

  //Order for finding DbSpec File:
  //(1) DbSpec file defined directly on command line - if not set then:
  //(2) DbSpec file set in environment variable (GROSS_DBSPEC) - if not set then:
  //(3) DbSpec file defined in $BOSSDIR/BossConfig.clad

  //Look for Environment variable (if DbSpec not explicitly set)
  if(opt_["-dbSpec"].empty()) {
    char* pc = getenv("GROSS_DBSPEC");
    if(pc) {
      opt_["-dbSpec"] = pc;
      if(Log::level()>0) cout <<"Taking DbSpec file set in GROSS_DBSPEC environment variable "<<opt_["-dbSpec"]<<endl;
    }
    else {
      char* pc = getenv("BOSSDIR");
      if(pc) {
	opt_["-dbSpec"] = pc+string("/BossConfig.clad");
	if(Log::level()>0) cout <<"Taking DbSpec file set in BOSSDIR env variable "<<opt_["-dbSpec"]<<endl;
      }
      else {
 	cerr<<"Error : DB spec file not set and not found as environment variable GROSS_DBSPEC or BOSSDIR"<<endl;
	return EXIT_FAILURE;
      }
    }
  }

  File myDbSpecFile(opt_["-dbSpec"]);
  if(!myDbSpecFile.exists()) {    
    cerr<< "Error : Cannot find db spec file " << myDbSpecFile.fullHandle()<<endl;
    return EXIT_FAILURE;
  }
  DbSpec* dbSpec_ = new DbSpec(myDbSpecFile.contents()); //Deleted in destructor (eg ~Prepare)
  if(LocalDb::instance()->init(dbSpec_)) {
    cerr << "Error : Failure connecting to db"<<endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
string Command::getFacType(int myTaskId) const {
  if(!myTaskId) return "";
  ostringstream sel;
  sel <<"ID="<<myTaskId;
  vector<string> results;
  if(LocalDb::instance()->tableRead("Analy_Task", "FacType", sel.str(), results)) return "";
  if(results.size()!=1) return "";
  return results[0];
}
