#include "OrcaGJob.hh"

#include "Task.hh"
#include "Wrapper.hh"
#include "JDL.hh"
#include "LocalDb.hh"
#include "FileSys.hh"
#include "File.hh"
#include "CladLookup.hh"
#include "ContPrint.hh"

#include "PhysCat.hh"

OrcaGJob::OrcaGJob(const int anId, const vector<int> aDataSelect, Task* aTask) : Job(anId, aDataSelect, aTask), pTarFile_(0)  {};

OrcaGJob::~OrcaGJob() {
  if(pTarFile_) delete pTarFile_;
}

int OrcaGJob::makeSubFiles() {}

int OrcaGJob::save() {
  //Save Base class stuff - must be called first (checks ID in DB)
  if(Job::save()) return EXIT_FAILURE;
  
  //Now OrcaGJob specific stuff
  ostringstream mySel;
  mySel << "TaskID="<<task_->Id() <<"&&JobID="<<Id();
  
  ostringstream myKeys, myValues, mvSelection;
  
  //Analy_Job
  if(LocalDb::instance()->tableUpdate("Analy_Job", "orcaVers", string("\"") + orcaVers_ + string("\""), mySel.str())) return EXIT_FAILURE;
  
  //InGUIDs Table
  myKeys.str("");
  myKeys << "TaskID, JobID, Name";
  /*for (set<string>::const_iterator i=(inGUIDs()).begin(); i!= (inGUIDs()).end(); i++){
    myValues.str("");
    myValues << task_->Id() << "," << Id() << ",'"<< (*i)<<"'";
    if(LocalDb::instance()->tableSave("Analy_InGUIDs", myKeys.str(), myValues.str())) return EXIT_FAILURE;
  }*/
  //OutGUIDs Table
  for (set<string>::const_iterator i=(outGUIDs()).begin(); i!= (outGUIDs()).end(); i++){
    myValues.str("");
    myValues << task_->Id() << "," << Id() << ",'"<< (*i)<<"'";
    if(LocalDb::instance()->tableSave("Analy_OutGUIDs", myKeys.str(), myValues.str())) return EXIT_FAILURE;
  }
  //OutSandbox Table
  for (set<string>::const_iterator i=(outSandboxFiles()).begin(); i!= (outSandboxFiles()).end(); i++){
    myValues.str("");
    myValues << task_->Id() << "," << Id() << ",'"<< (*i)<<"'";
    if(LocalDb::instance()->tableSave("Analy_OutSandbox", myKeys.str(), myValues.str())) return EXIT_FAILURE;
  }
    
  return EXIT_SUCCESS;
}
int OrcaGJob::clean() {
  //Clean Base class stuff
  if(Job::clean()) return EXIT_FAILURE;

  //Clean OrcaGJob stuff
  ostringstream sel;
  sel << "TaskID="<<task_->Id()<<"&&JobID="<<Id();

  if(/*LocalDb::instance()->tableDelete("Analy_InGUIDs", sel.str()) ||*/
     LocalDb::instance()->tableDelete("Analy_OutGUIDs", sel.str()) || 
     LocalDb::instance()->tableDelete("Analy_OutSandbox", sel.str()) ) {
    cerr << "Job::delete: Error deleting previous entry!"<<endl;
    return EXIT_FAILURE;
  }
    
  return EXIT_SUCCESS; 
}

void OrcaGJob::print() const {
  Job::print();
  /*cout << "Input Data File LFNs = " << inGUIDs() <<endl;*/
  cout << "Output Sandbox Files = " << outSandboxFiles()<<endl;
  cout << "Output Data File LFNs = " << outGUIDs() <<endl<<endl;
}
