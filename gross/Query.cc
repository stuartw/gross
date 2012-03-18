#include "Query.hh"

#include "QInfoTask.hh"
#include "Task.hh"
#include <sstream>
#include "LocalDb.hh"
#include "Range.hh"
#include "TaskFactory.hh"

Query::Query() : Command() {
  opt_["-taskId"] = "0";
  opt_["-jobId"] = "0";
  opt_["-dbSpec"];
  opt_["-logLevel"]  = "0";
  opt_["-taskInfo"];
  opt_["-jobInfo"];
  opt_["-dataQ"];
  opt_["-status"];
  opt_["-sumStat"];
  opt_["-printAll"];
  opt_["-allTasks"];
  opt_["-bossDb"] ="";
  opt_["-noHead"];
}
Query::~Query(){
}

void Query::printUsage() const
{
  cerr << "Usage:" << endl
       << "gross query " << endl;
  cerr
       << "            -taskId <taskID>" << endl
       << "            -jobId <GROSS Job ID range>" << endl
       << "            -dbSpec <dbSpecFile>" << endl
       << "            -taskInfo <task level information>" << endl
       << "            -jobInfo <job level information>" << endl
       << "            -status <job status>" <<endl
       << "            -sumStat <summary job status>" <<endl
       << "            -dataQ <meta data query>" <<endl
       << "            -bossDb <BOSS Db query>" <<endl
       << "            -printAll <sets all print options above>" <<endl 
       << "            -allTasks <all tasks printed>" <<endl 
       << "            -noHead <turns readable headers off>" <<endl
       << "            -logLevel <logLevel>" << endl    
       << endl;
}
int Query::execute() {

  //Logging Verbosity
  Log::level(atoi((opt_["-logLevel"].c_str())));
  if(Log::level()>0) cout <<"Logging Level Set to Verbose level "<<Log::level()<<endl;

  //Initialise Db
  if(initDb()) return EXIT_FAILURE;
  
  //Fill task vector up with task IDs to be processed
  if(opt_["-allTasks"]=="TRUE") allTasks();

  if(opt_["-taskId"]!="0") {
    std::istringstream is(opt_["-taskId"]);
    int taskId;
    is>>taskId;
    taskIds_.push_back(taskId);
  }
  
  if(taskIds_.empty()) return EXIT_FAILURE;

  bool first=true;
  for(vector<int>::const_iterator it=taskIds_.begin(); it!=taskIds_.end(); ++it) {

    //Initialise factory appropriately for this task 
    TaskFactory::del(); //ensure last concrete factory instance is deleted.
    TaskFactory::facType(getFacType(*it)); //set concrete factory type
    
    //Create QInfoTask object for particular task/jobs required
    Range jobRange(opt_["-jobId"]);
    
    QInfoTask myQInfoTask; //create local v'ble so it will get deleted at end of scope.
    if(opt_["-noHead"] =="TRUE"||!first) myQInfoTask.header(false);
    if(myQInfoTask.init(*it, jobRange.min(), jobRange.max())) continue;
    
    if(opt_["-taskInfo"]=="TRUE" || opt_["-printAll"]=="TRUE")
      if(myQInfoTask.printTask()) continue;
    
    if(opt_["-jobInfo"]=="TRUE" || opt_["-printAll"]=="TRUE")
      if(myQInfoTask.printJobs()) continue;

    if(opt_["-dataQ"]=="TRUE" || opt_["-printAll"]=="TRUE")
      if(myQInfoTask.printDataQuery()) continue;  
    
    if(opt_["-status"]=="TRUE" || opt_["-printAll"]=="TRUE")
      if(myQInfoTask.printStatus()) continue;

    if(opt_["-sumStat"]=="TRUE" || opt_["-printAll"]=="TRUE")
      if(myQInfoTask.printSumStatus()) continue;  
        
    if(!opt_["-bossDb"].empty())
      if(myQInfoTask.printBossDbField(opt_["-bossDb"])) continue;
    
    first=false;
  }
    
  return EXIT_SUCCESS;
}


void Query::allTasks() {
  vector<string> myResults;
  LocalDb::instance()->tableRead("Analy_Task", "ID", "ID>0", myResults);
  for(vector<string>::const_iterator it = myResults.begin(); it!=myResults.end(); ++it) {
    int taskId;
    std::istringstream is(*it);    
    is>>taskId;
    taskIds_.push_back(taskId);
  }
}
