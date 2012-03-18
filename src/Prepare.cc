#include "Prepare.hh"

#include "TaskFactory.hh"
#include "Task.hh"
#include "FileSys.hh"
#include "File.hh"

Prepare::Prepare() : Command() {
  opt_["-taskId"] = "0";
  opt_["-oDir"]  = FileSys::workingDir();
  opt_["-userSpec"]  = "NULL";
  opt_["-dbSpec"];
  opt_["-save"] = "TRUE";
  opt_["-type"]  = "OrcaG";
  opt_["-logLevel"]  = "0";
}
Prepare::~Prepare(){}

void Prepare::printUsage() const
{
  cerr << "Usage:" << endl
       << "gross prepare " << endl;
  cerr
       << "            -oDir <outputDirectory>" << endl
       << "            -userSpec <userSpecFile>" << endl
       << "            -dbSpec <dbSpecFile>" << endl
       << "            -save" << endl
       << "            -type <wrapperName>" << endl
       << "            -logLevel <logLevel>" << endl
       << endl;
}
int Prepare::execute() {

  taskId_ = 0; //cant do in constructor

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
  Task* myTask=0;

  //.. either from database:
  taskId_=atoi((opt_["-taskId"]).c_str());	  
  if(taskId_) {
    TaskFactory::facType(getFacType(taskId_));
    myTask=(TaskFactory::instance())->makeTask(taskId_);
  }
  //.. or from new user spec
  else
    {
      TaskFactory::facType(opt_["-type"]);  
      File userFile(opt_["-userSpec"]);
      myTask=(TaskFactory::instance())->makeTask(&userFile);
    }
  if(!myTask) return EXIT_FAILURE;  //check ptr exists
  if(!*myTask) return EXIT_FAILURE; //check object was initialised ok
  
  //Prepare jobs
  cout << "Preparing jobs for this task "<<endl;
  if(myTask->split()) return EXIT_FAILURE;
  if(myTask->makeSubFiles()) return EXIT_FAILURE;
  
  //Save task and jobs to db
  if(opt_["-save"]=="TRUE") {
    cout <<"Saving task info to db"<<endl;
    taskId_ = myTask->save();
    if(!taskId_) return EXIT_FAILURE;
    cout <<"Task and Job info saved to database with Task ID "<< taskId_ <<endl;
  }
  if(myTask->delSubFiles()) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

