#include "Kill.hh"

#include "LocalDb.hh"
#include <sstream>
#include <vector>
#include "BossIf.hh"
#include "TaskFactory.hh"
#include "Task.hh"

Kill::Kill() : Command() {
  opt_["-taskId"]="0";
  opt_["-jobId"]="0";
  opt_["-dbSpec"];
  opt_["-logLevel"]  = "0";
}
Kill::~Kill(){
}

void Kill::printUsage() const
{
  cerr << "Usage:" << endl
       << "gross kill " << endl;
  cerr
       << "            -taskId <taskID>" << endl
       << "            -jobId <GROSS job ID>" << endl
       << "            -dbSpec <dbSpecFile>" << endl
       << "            -logLevel <logLevel>" << endl    
       << endl;
}
int Kill::execute() {

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
  cout << "Killing submitted jobs of task "<<myTaskId;
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
  
  //perform BOSS kill from scheduler for single job or whole task
  BossIf boss(pTask);
  if (myJobId!=0) {
    if (boss.killJob(myJobId)) cerr<<"Error killing job" << myJobId<< " from BOSS "<<endl;
  }
  else {
    if (boss.killTask()) cerr<<"Error killing Task " << myTaskId<< " from BOSS "<<endl;
  }
  
      if(Log::level()>0) cout << "Kill successful for Task " << myTaskId<< endl;

  return EXIT_SUCCESS;
}


