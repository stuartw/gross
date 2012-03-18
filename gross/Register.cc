#include "Register.hh"

#include "File.hh"
#include "DbSpec.hh"
#include "LocalDb.hh"

Register::Register() : Command() {
  opt_["-dbSpec"];
  opt_["-wrapScript"] = "NULL";
  opt_["-wrapName"] = "NULL";
  opt_["-logLevel"]  = "0";
  dbSpec_=0;
}
Register::~Register(){
  if(dbSpec_) delete dbSpec_;
}

void Register::printUsage() const
{
  cerr << "Usage:" << endl
       << "gross register " << endl;
  cerr
       << "            -dbSpec <DbSpecFile>" << endl
       << "            -wrapScript <WrapperScript>" << endl
       << "            -wrapName <WrapperName>" << endl
       << "            -logLevel <logLevel>" << endl
       << endl;
}
int Register::execute() {

  //Logging Verbosity
  Log::level(atoi((opt_["-logLevel"].c_str())));
  if(Log::level()>0) cout <<"Logging Level Set to Verbose level "<<Log::level()<<endl;

  //Initialise Db
  if(initDb()) return EXIT_FAILURE;     

  //Check input
  if(opt_["-wrapScript"] =="NULL" || opt_["-wrapName"]=="NULL") {
    cerr<<"Error: -wrapScript and/or -wrapName unspecified"<<endl;
    return EXIT_FAILURE;
  }
  
  //Find Specified Wrap Script
  File wrapScript(opt_["-wrapScript"]);
  if(!wrapScript.exists()) {
    cerr<<"Error: Wrapper Script not found"<<endl;
    return EXIT_FAILURE;
  }

  //Get escaped string of the script
  string convS = LocalDb::instance()->escapeString(wrapScript.contents());
  if(convS.empty()) return EXIT_FAILURE;
  
  //Insert into database (over-writing any previous definition)
  ostringstream query;
  query <<"REPLACE INTO Analy_Wrapper (name, script) VALUES ('"
	<< opt_["-wrapName"] << "',"
	<< convS <<")"; 

  if(Log::level()>2) cout <<"Register::execute query to Db is "<<query.str();
  if(mysql_query(LocalDb::instance()->connection(), (query.str()).c_str())) {
    cerr << "Error saving Wrapper to Db" 
	 << mysql_error(LocalDb::instance()->connection()) << endl;
    return EXIT_FAILURE;
  }
  cout <<"Script saved successfully"<<endl;
  
  
  return EXIT_SUCCESS;
}
