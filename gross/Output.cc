#include "Output.hh"

#include "TaskOutput.hh"
#include "Task.hh"
#include "FileSys.hh"
#include "Range.hh"

Output::Output() : Command() {
  opt_["-taskId"] = "0";
  opt_["-jobId"] = "0";
  opt_["-dbSpec"];
  opt_["-oDir"]  = "./";
  opt_["-logLevel"]  = "0";
  opt_["-sandbox"];
}
Output::~Output(){}

void Output::printUsage() const
{
  cerr << "Usage:" << endl
       << "gross output " << endl;
  cerr
       << "            -taskId <taskID>" << endl
       << "            -jobId <GROSS Job ID range>" << endl
       << "            -dbSpec <dbSpecFile>" << endl
       << "            -sandbox <retrieve output sandbox files>" <<endl 
       << "            -oDir <outputDirectory to retrieve files to>" << endl
       << "            -logLevel <logLevel>" << endl    
       << endl;
}
int Output::execute() {

  //Logging Verbosity
  Log::level(atoi((opt_["-logLevel"].c_str())));
  if(Log::level()>0) cout <<"Logging Level Set to Verbose level "<<Log::level()<<endl;

  //Initialise Db
  if(initDb()) return EXIT_FAILURE;

  //Output file directory
  if(FileSys::oDir(opt_["-oDir"])) {
    cerr<<"Error: Specified output directory does not exist"<<endl;
    return EXIT_FAILURE;
  }
  
  //Create QInfoTask object for particular task/jobs required
  int myTaskId=atoi((opt_["-taskId"]).c_str());	  

  Range jobRange(opt_["-jobId"]);    

  TaskOutput myTaskOutput;
  if(myTaskOutput.init(myTaskId, jobRange.min(), jobRange.max())) return EXIT_FAILURE;


  if(opt_["-sandbox"]=="TRUE")
    if(myTaskOutput.printSboxDirs(FileSys::oDir())) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
