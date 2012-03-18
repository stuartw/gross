#include "Prepare.hh"

#include "TaskFactory.hh"
#include "Task.hh"
#include "FileSys.hh"
#include "File.hh"

Prepare::Prepare() : Command() {
  opt_["-taskId"] = "0";
  opt_["-oDir"]  = "./";
  opt_["-userSpec"]  = "NULL";
  opt_["-dbSpec"];
  opt_["-save"];
  opt_["-wrapName"]  = "standard";
  opt_["-logLevel"]  = "0";
}
Prepare::~Prepare(){}

void Prepare::printUsage() const
{
  cerr << "Usage:" << endl
       << "gross prepare " << endl;
  cerr
       << "            -taskId <taskID>" << endl
       << "            -oDir <outputDirectory>" << endl
       << "            -userSpec <userSpecFile>" << endl
       << "            -dbSpec <dbSpecFile>" << endl
       << "            -save" << endl
       << "            -wrapName <wrapperName>" << endl
       << "            -logLevel <logLevel>" << endl
       << endl;
}
int Prepare::execute() {

  //Logging Verbosity
  Log::level(atoi((opt_["-logLevel"].c_str())));
  if(Log::level()>0) cout <<"Logging Level Set to Verbose level "<<Log::level()<<endl;

  //Output file directory
  if(FileSys::oDir(opt_["-oDir"])) {
    cerr<<"Error: Specified output directory does not exist"<<endl;
    return EXIT_FAILURE;
  }

  //Initialise Db (if necessary)
  if(initDb()) return EXIT_FAILURE;
  
  //Create Task
  int myTaskId=atoi((opt_["-taskId"]).c_str());	  
  Task* myTask=(TaskFactory::instance())->makeTask(myTaskId);
  if(!myTask) return EXIT_FAILURE;
  File userfile(opt_["-userSpec"]);

  //Initialise Task
  if(myTask->init(&userfile, myTaskId)) return EXIT_FAILURE;

  //Prepare jobs
  cout << "Preparing jobs for this task "<<endl;
  if(myTask->prepareJobs(opt_["-wrapName"])) return EXIT_FAILURE;

  //Save task and jobs to db
  if(opt_["-save"]=="TRUE") {
    cout <<"Saving task info to db"<<endl;
    myTaskId = myTask->save();
    if(!myTaskId) return EXIT_FAILURE;
    cout <<"Task and Job info saved to database with Task ID "<< myTaskId <<endl;
  }
  
  return EXIT_SUCCESS;
}
