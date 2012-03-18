#include "Submit.hh"

#include "TaskFactory.hh"
#include "BossIf.hh"
#include "Task.hh"
#include "FileSys.hh"
#include "Range.hh"

Submit::Submit() : Command() {
  opt_["-taskId"] = "0";
  opt_["-oDir"]  = FileSys::workingDir();
  opt_["-dbSpec"];
  opt_["-bossType"]  = "gross";
  opt_["-bossSched"]  = "edg";
  opt_["-jobId"]  = "NULL";
  opt_["-resubmit"] = "NULL";
  opt_["-logLevel"]  = "0";
}
Submit::~Submit(){}

void Submit::printUsage() const
{
  cerr << "Usage:" << endl
       << "gross submit " << endl;
  cerr
       << "            -taskId <taskID>" << endl
       << "            -oDir <outputDirectory>" << endl
       << "            -dbSpec <dbSpecFile>" << endl
       << "            -bossType <BossJobType>" <<endl
       << "            -bossSched <BossScheduler>" <<endl
       << "            -jobId <GROSS Job ID range>" <<endl
       << "	       -resubmit" << endl
       << "            -logLevel <logLevel>" << endl
       << endl;
}
int Submit::execute() {

  //Logging Verbosity
  Log::level(atoi((opt_["-logLevel"].c_str())));
  if(Log::level()>0) cout <<"Logging Level Set to Verbose level "<<Log::level()<<endl;

  //Output file directory
  if(FileSys::oDir(opt_["-oDir"])) {
    cerr<<"Error: Specified output directory does not exist"<<endl;
    return EXIT_FAILURE;
  }

  //Initialise Db
  if(initDb()) return EXIT_FAILURE;     

  //Create Task
  int myTaskId = atoi((opt_["-taskId"]).c_str());
  if(!myTaskId) {
    cerr<<"Error: -taskId not specified"<<endl;
    return EXIT_FAILURE;
  }
  cout <<"Reading from Db task ID "<<opt_["-taskId"]<<endl;
  TaskFactory::facType(getFacType(myTaskId)); //Initialise Factory type		       
  Task* pTask=(TaskFactory::instance())->makeTask(myTaskId); //Create task
  if(!pTask) return EXIT_FAILURE;  //check ptr exists
  if(!*pTask) return EXIT_FAILURE; //check object was initialised ok

  //Prepare jobs
  cout << "Preparing jobs for this task "<<endl;
  if(pTask->split()) return EXIT_FAILURE;
  if(pTask->makeSubFiles()) return EXIT_FAILURE;

  //Submit jobs
  Range jobRange(opt_["-jobId"]);
   
  cout << "Submitting Jobs"<<endl;
  BossIf myBossIf(pTask); //Create BOSSIf object to submit to
  if (myBossIf.setJobs(jobRange, opt_["-resubmit"]=="TRUE")) return EXIT_FAILURE; 
  
  if(myBossIf.submitJobs(opt_["-bossSched"],opt_["-bossType"], jobRange)) return EXIT_FAILURE;
  
  //delete submission files
  if (Log::level()>0) cout <<"Deleting submission files"<<endl;
  if(pTask->delSubFiles()) return EXIT_FAILURE;
  
  return EXIT_SUCCESS;
}
