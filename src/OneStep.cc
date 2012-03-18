#include "OneStep.hh"
#include "LocalDb.hh"
#include "FileSys.hh"
#include "Prepare.hh"
#include "Submit.hh"
#include "Output.hh"

OneStep::OneStep() : Command() {
  opt_["-dbSpec"];
  opt_["-logLevel"]  = "0";
  opt_["-oDir"]  = FileSys::workingDir();
  opt_["-userSpec"]  = "gross.jdl";
  opt_["-type"]  = "OrcaG";
  opt_["-bossType"]  = "gross";
  opt_["-bossSched"]  = "edg";
  opt_["-jobId"]  = "NULL";    
  opt_["-save"] = "TRUE";
  opt_["-autoRetrieve"] = "TRUE";
  opt_["-outputDir"] = FileSys::workingDir();
  
  prepare_ = new Prepare();
  submit_ = new Submit();
  output_ = new Output();
}
OneStep::~OneStep(){
  if (prepare_) delete prepare_;
  if (submit_) delete submit_;
  if (output_) delete output_;
}

void OneStep::printUsage() const
{
  cerr << endl;
  cerr << "Usage:" << endl
       << "gross onestep " << endl;
  cerr
       << "	Prepare Options" << endl
       << "            -userSpec <userSpecFile>" << endl
       << "            -type <wrapperName>" << endl 
       << endl
       << "	Submit Options" << endl
       << "            -bossType <BossJobType>" <<endl
       << "            -bossSched <BossScheduler>" <<endl
       << "            -jobId <Gross jobId range>" <<endl	       
       << endl
       << "	Output Options" <<endl
       << "            -outputDir <outputDirectory>" <<endl
       << endl
       << "	General Options" << endl
       << "            -dbSpec <dbSpecFile>" << endl
       << "            -logLevel <logLevel>" << endl
       << "            -oDir <workingDirectory>" << endl
       << endl;
}
int OneStep::execute() {

  //Logging Verbosity
  Log::level(atoi((opt_["-logLevel"].c_str())));
  if(Log::level()>0) cout <<"Logging Level Set to Verbose level "<<Log::level()<<endl;   
	
  //do Job preparation
  prepare_->acceptOptions(opt_);
  if(prepare_->execute()) {
    cerr << "Error: Unable to prepare task" <<endl;
    return EXIT_FAILURE;
  }
 
  //retrieve taskId from DB
  taskId = 0;
  taskId = LocalDb::instance()->maxCol("Analy_Task","ID");
  if (!taskId) {
    cerr << "Error: Unable to retreive task from database" << endl;
    return EXIT_FAILURE;
  }

  ostringstream os;
  os << taskId;
  opt_["-taskId"] = os.str();
 
  //submit task
  submit_->acceptOptions(opt_);
  if(submit_->execute()) {
    cerr << "Error: Unable to submit task" << endl;
    return EXIT_FAILURE;
  }

  //start auto retrieve for grid/local jobs
  if (opt_["-type"]=="OrcaG"||opt_["-type"]=="OrcaLoc") {
    opt_["-oDir"] = opt_["-outputDir"];
    output_->acceptOptions(opt_);  
    if(output_->execute()) {
      cerr << "Error: Unable to do autoretrieval task" << endl;
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}


