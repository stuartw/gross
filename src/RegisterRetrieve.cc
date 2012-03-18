#include "RegisterRetrieve.hh"

#include "File.hh"
#include "DbSpec.hh"
#include "LocalDb.hh"

RegisterRetrieve::RegisterRetrieve() : Command() {
  opt_["-dbSpec"];
  opt_["-script"] = "NULL";
  opt_["-scriptName"] = "NULL";
  opt_["-logLevel"]  = "0";
  dbSpec_=0;
}
RegisterRetrieve::~RegisterRetrieve(){
  if(dbSpec_) delete dbSpec_;
}

void RegisterRetrieve::printUsage() const
{
  cerr << "Usage:" << endl
       << "gross registerRetrieve " << endl;
  cerr
       << "            -dbSpec <DbSpecFile>" << endl
       << "            -script <Script>" << endl
       << "            -scriptName <ScriptName>" << endl
       << "            -logLevel <logLevel>" << endl
       << endl;
}
int RegisterRetrieve::execute() {

  //Logging Verbosity
  Log::level(atoi((opt_["-logLevel"].c_str())));
  if(Log::level()>0) cout <<"Logging Level Set to Verbose level "<<Log::level()<<endl;

  //Initialise Db
  if(initDb()) return EXIT_FAILURE;     

  //Check input
  if(opt_["-script"] =="NULL") {
    cerr<<"Error: -script file unspecified"<<endl;
    return EXIT_FAILURE;
  }
  
  if(opt_["-scriptName"]=="NULL") {
    cerr<<"Error: -scriptName unspecified"<<endl;
    return EXIT_FAILURE;
  }

  //Find Specified Wrap Script
  File script(opt_["-script"]);
  if(!script.exists()) {
    cerr<<"Error: Script not found"<<endl;
    return EXIT_FAILURE;
  }

  //Get escaped string of the script
  string convS = LocalDb::instance()->escapeString(script.contents());
  if(convS.empty()) return EXIT_FAILURE;
  
  //Insert into database (over-writing any previous definition)
  ostringstream query;
  query <<"REPLACE INTO Analy_Retrieve (name, script) VALUES ('"
	<< opt_["-scriptName"] << "',"
	<< convS <<")"; 

  if(Log::level()>2) cout <<"RegisterRetrieve::execute query to Db is "<<query.str();
  if(mysql_query(LocalDb::instance()->connection(), (query.str()).c_str())) {
    cerr << "Error saving script to Db" 
	 << mysql_error(LocalDb::instance()->connection()) << endl;
    return EXIT_FAILURE;
  }
  cout <<"Script saved successfully"<<endl;
  
  
  return EXIT_SUCCESS;
}
