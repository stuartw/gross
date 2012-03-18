#include "Delete.hh"

#include "LocalDb.hh"
#include <sstream>
#include <vector>
#include "BossIf.hh"
#include "TaskFactory.hh"
#include "Task.hh"

Delete::Delete() : Command() {
  opt_["-taskId"]="0";
  opt_["-jobId"]="0";
  opt_["-dbSpec"];
  opt_["-logLevel"]  = "0";
}
Delete::~Delete(){
}

void Delete::printUsage() const
{
  cerr << "Usage:" << endl
       << "gross delete " << endl;
  cerr
       << "            -taskId <taskID>" << endl
       << "            -jobId <GROSS job ID>" << endl
       << "            -dbSpec <dbSpecFile>" << endl
       << "            -logLevel <logLevel>" << endl    
       << endl;
}
int Delete::execute() {

  //Logging Verbosity
  Log::level(atoi((opt_["-logLevel"].c_str())));
  if(Log::level()>0) cout <<"Logging Level Set to Verbose level "<<Log::level()<<endl;

  //Initialise Db
  if(initDb()) return EXIT_FAILURE;
  
  int myTaskId = atoi((opt_["-taskId"]).c_str());
  if(myTaskId==0) {
    cerr<<"Error: -taskId not specified"<<endl;
    return EXIT_FAILURE;
  }
 
  //Prepare SQL query
  ostringstream sel;
  cout << "Deleting task "<<myTaskId;
  sel << "TaskID="<<myTaskId;
  
  int myJobId = atoi((opt_["-jobId"]).c_str());
  if(myJobId!=0) {
    cout <<" and GROSS Job ID "<<myJobId;
    sel << "&&JobID=" << myJobId;
  }
  cout <<endl;
  
  TaskFactory::facType(getFacType(myTaskId)); //Initialise Factory type
  Task* pTask=(TaskFactory::instance())->makeTask(myTaskId); //Create task
  if(!pTask) return EXIT_FAILURE;  //check ptr exists
  if(!*pTask) return EXIT_FAILURE; //check object was initialised ok
  
  //Load sub jobs
  if(pTask->split()) return EXIT_FAILURE;
  
  //Perform deletion from DB
  if((sel.str()).empty()) return EXIT_FAILURE;

  //Standard tables
  std::vector<string> myTables;
  myTables.push_back("Analy_Job");
  myTables.push_back("Analy_InGUIDs");
  myTables.push_back("Analy_LocalIn");
  myTables.push_back("Analy_OutGUIDs");
  myTables.push_back("Analy_OutSandbox");

  for(vector<string>::const_iterator it = myTables.begin(); it!=myTables.end(); ++it) {
    if(LocalDb::instance()->tableDelete(*it, sel.str())){
      cerr<<"Error Deleting from DB tablename "<<*it<<endl;}
    else {
      if(Log::level()>0) cout << "Delete successful from tablename " <<*it<<endl;
    }
  }

  //Task Table - has ID not TaskId field
  if(myJobId==0) {
    sel.str("");
    sel<<"ID="<<myTaskId;
    if(LocalDb::instance()->tableDelete("Analy_Task", sel.str())) {
      cerr<<"Error Deleting from DB tablename Analy_Task"<<endl;
    }
    else {
      if(Log::level()>0) cout << "Delete successful from tablename Analy_Task" <<endl;
    }
  }

  return EXIT_SUCCESS;
}


