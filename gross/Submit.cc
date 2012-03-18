#include "Submit.hh"

#include "TaskFactory.hh"
#include "BossIf.hh"
#include "Task.hh"
#include "FileSys.hh"

Submit::Submit() : Command() {
  opt_["-taskId"] = "0";
  opt_["-oDir"]  = "./";
  opt_["-dbSpec"];
  opt_["-bossType"]  = "NULL";
  opt_["-bossSched"]  = "NULL";
  opt_["-jobId"]  = "NULL";
  opt_["-wrapName"]  = "standard";
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
       << "            -jobId <jobId>" <<endl
       << "            -wrapName <type>" <<endl
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
  if(myTaskId==0) {
    cerr<<"Error: -taskId not specified"<<endl;
    return EXIT_FAILURE;
  }
  Task* pTask=(TaskFactory::instance())->makeTask(myTaskId);
  if(!pTask) return EXIT_FAILURE;
  cout <<"Reading from Db task ID "<<opt_["-taskId"]<<endl;
  File* nullFile;
  if(pTask->init(nullFile, myTaskId)) return EXIT_FAILURE;

  //Prepare jobs
  cout << "Preparing jobs for this task "<<endl;
  if(pTask->prepareJobs(opt_["-wrapName"])) return EXIT_FAILURE;

  //Submit jobs
  if(opt_["-bossType"]=="NULL" || opt_["-bossSched"]=="NULL") {
    cerr <<"Error: -bossType and/or -bossSched not specified"<<endl;
    return EXIT_FAILURE;
  }
  BossIf myBossIf(pTask); //Create object to submit to
  if(opt_["-jobId"]!="NULL") {
    int myJobId = atoi((opt_["-jobId"]).c_str());
    cout <<"Submitting single job to BOSS with GROSS JobId " << myJobId <<endl;
    //Get Job from Task
    const Job* pJob = pTask->job(myJobId);
    if(!pJob) {
      cerr<<"Error retrieving Job Id from Task"<<endl;
      return EXIT_FAILURE;
    }    
    if(myBossIf.submitJob(opt_["-bossSched"],opt_["-bossType"],pJob)) return EXIT_FAILURE;
  }
  else {
    cout <<"Submitting Task"<<endl;
    if(myBossIf.submitTask(opt_["-bossSched"],opt_["-bossType"])) return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
