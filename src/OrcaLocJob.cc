#include "OrcaLocJob.hh"

#include "Task.hh"
#include "Wrapper.hh"
#include "JDL.hh"
#include "LocalDb.hh"
#include "FileSys.hh"
#include "File.hh"
#include "CladLookup.hh"
#include "ContPrint.hh"

#include "PhysCat.hh"

OrcaLocJob::OrcaLocJob(const int anId, const vector<int> aDataSelect, Task* aTask) : Job(anId, aDataSelect, aTask), pTarFile_(0) {};

OrcaLocJob::~OrcaLocJob() {
  if(pTarFile_) delete pTarFile_;
}

int OrcaLocJob::makeSubFiles() {}

int OrcaLocJob::save() {
  //Save Base class stuff - must be called first (checks ID in DB)
  if(Job::save()) return EXIT_FAILURE;
  
  //Now OrcaLocJob specific stuff
  ostringstream mySel;
  mySel << "TaskID="<<task_->Id() <<"&&JobID="<<Id();
  
  ostringstream myKeys, myValues;
  
  //Analy_Job
  if(LocalDb::instance()->tableUpdate("Analy_Job", "orcaVers", string("\"") + orcaVers_ + string("\""), mySel.str())) return EXIT_FAILURE;
  
  //InFiles Table
  /*myKeys.str("");
  myKeys << "TaskID, JobID, Name";
  for (set<string>::const_iterator i=(inFiles()).begin(); i!= (inFiles()).end(); i++){
    myValues.str("");
    myValues << task_->Id() << "," << Id() << ",'"<< (*i)<<"'";
    if(LocalDb::instance()->tableSave("AnalyLoc_InFiles", myKeys.str(), myValues.str())) return EXIT_FAILURE;
  }*/
  //OutFiles Table
  for (set<string>::const_iterator i=(outFiles()).begin(); i!= (outFiles()).end(); i++){
    myValues.str("");
    myValues << task_->Id() << "," << Id() << ",'"<< (*i)<<"'";
    if(LocalDb::instance()->tableSave("AnalyLoc_OutFiles", "TaskID, JobID, Name", myValues.str())) return EXIT_FAILURE;
  }
  //outDir to Analy_Job table
  ostringstream select;
  myValues.str("");
  myValues << "'" << outDir() << "'";
  select << "TaskID=" << task_->Id() <<"&&JobID="<<Id();
  if(LocalDb::instance()->tableUpdate("Analy_Job", "outDir", myValues.str(), select.str())) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
int OrcaLocJob::clean() {
  //Clean Base class stuff
  if(Job::clean()) return EXIT_FAILURE;

  //Clean OrcaGJob stuff
  ostringstream sel;
  sel << "TaskID="<<task_->Id()<<"&&JobID="<<Id();

  if(LocalDb::instance()->tableDelete("AnalyLoc_InFiles", sel.str()) ||
     LocalDb::instance()->tableDelete("AnalyLoc_OutFiles", sel.str())) {
    cerr << "Job::delete: Error deleting previous entry!"<<endl;
    return EXIT_FAILURE;
  }
    
  return EXIT_SUCCESS; 
}

void OrcaLocJob::print() const {
  Job::print();
  //cout << "Input Data File's = " << inFiles() <<endl;
  cout << "Output Data File's = " << outFiles() <<endl;
}
