#include "Output.hh"

#include "TaskOutput.hh"
#include "Task.hh"
#include "FileSys.hh"
#include "Range.hh"
#include "TaskFactory.hh"

Output::Output() : Command() {
  opt_["-taskId"] = "0";
  opt_["-jobId"] = "0";
  opt_["-dbSpec"];
  opt_["-oDir"]  = FileSys::workingDir();
  opt_["-logLevel"]  = "0";
  opt_["-sandbox"] = "TRUE";
  opt_["-autoRetrieve"];
  opt_["-cancel"];
  opt_["-cont"];
  cont=init=0;
}
Output::~Output(){}

void Output::printUsage() const
{
  if (cont&&init) return;
    cerr << "Usage:" << endl
         << "gross output " << endl;
    cerr
         << "            -taskId <taskID>" << endl
         << "            -jobId <GROSS Job ID range>" << endl
         << "            -dbSpec <dbSpecFile>" << endl
         << "            -oDir <outputDirectory to retrieve files to>" << endl
         << "            -logLevel <logLevel>" << endl    
         << "            -autoRetrieve" << endl
         << "            -cancel" << endl
         << endl;
  
}
int Output::execute() {
  
  //used to remove usage when auto retrieve is on
  cont = (opt_["-cont"]=="TRUE");
  init = true;

  //Logging Verbosity
  Log::level(atoi((opt_["-logLevel"].c_str())));
  if(Log::level()>0) cout <<"Logging Level Set to Verbose level "<<Log::level()<<endl;

  //Initialise Db
  if(initDb()) return EXIT_FAILURE;

  //Output file directory
  if(FileSys::oDir(opt_["-oDir"],true)) {
    cerr<<"Error: directory " << opt_["-oDir"] << " does not exist, or cannot be created"<<endl;
    return EXIT_FAILURE;
  }
  //Create QInfoTask object for particular task/jobs required
  int myTaskId=atoi((opt_["-taskId"]).c_str());	  

  //Initialise concrete factory type
  TaskFactory::facType(getFacType(myTaskId));

  Range jobRange(opt_["-jobId"]);    

  TaskOutput myTaskOutput;
  if(myTaskOutput.init(myTaskId, jobRange.min(), jobRange.max())) return EXIT_FAILURE;

  if(opt_["-autoRetrieve"]=="TRUE") {
    if(myTaskOutput.setAuto(FileSys::oDir())) return EXIT_FAILURE;
    return EXIT_SUCCESS;
  }
  
  if(opt_["-cancel"]=="TRUE") {
    if(myTaskOutput.cancelAuto()) return EXIT_FAILURE;
    return EXIT_SUCCESS;
  }
  
  if(myTaskOutput.setFinJobs()) return EXIT_FAILURE;
  
  if(opt_["-sandbox"]=="TRUE") {
    if(myTaskOutput.printSboxDirs(FileSys::oDir())) return EXIT_FAILURE;
    //if(myTaskOutput.recoverJobs(FileSys::oDir())) return EXIT_FAILURE;
  }
    
  if(opt_["-cont"]=="TRUE")
    return myTaskOutput.cont(); 
 
  return EXIT_SUCCESS;
}
