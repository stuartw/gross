#include "NewOrcaTask.hh"

#include "TaskFactory.hh"
#include "PhysCat.hh"
#include "Job.hh"
#include "CladLookup.hh"
#include "ContPrint.hh"
#include "Range.hh"

NewOrcaTask::~NewOrcaTask() {
  if(physCat_) delete physCat_;
}

int NewOrcaTask::split(){

  //read in owner and dataset
  const string sOwner = userSpec()->read("Owner");
  const string sDataset = userSpec()->read("Dataset");
  const string sDataChain = userSpec()->read("DataChain");
  if(sOwner.empty()||sDataset.empty()) {
    cerr <<"NewOrcaTask::split() Error: Dataset Owner and/or Dataset name not defined in JDL (Owner=?/Dataset=?)"<<endl;
    return EXIT_FAILURE;
  }
  
  //Initialise catalogue - this will create temporary local cat
  physCat(new PhysCat(sOwner, sDataset, sDataChain));  //deleted in ~NewOrcaTask
  
  int runsPerJob = atoi((userSpec()->read("RunsPerJob")).c_str());
  if (runsPerJob<=0) { 
    if(Log::level()>0) cout<< "NewOrcaTask::split() Warning: RunsPerJob not correctly defined - set to default value (1)"<<endl;
    runsPerJob=1;
  }
  
  vector<int> runs_ = physCat()->getAllRuns();
  int jobId=1;
  for(vector<int>::const_iterator i = runs_.begin(); i<runs_.end()  ; i+= runsPerJob) {
    //create sub vector of runs specific to that job - if not enough runs left then make job from however many are left
    vector<int> myRuns; 
    if (runs_.end()>i+runsPerJob)  myRuns=vector<int>(i, i+runsPerJob);
    else  myRuns=vector<int>(i, i+(runs_.end()-i));
  
    if(Log::level()>-1) cout <<"Creating job with id " << jobId << " for data selection (runs) " << myRuns <<endl;
    
    Job* pJ = (TaskFactory::instance())->makeJob(jobId, myRuns, this);
    if(!pJ) return EXIT_FAILURE; //check pointer exists
    if(!*pJ) return EXIT_FAILURE; //check object constructed ok    
    jobs_.push_back(pJ);   //deleted in ~Task
    jobId++;
  }

  if(jobs_.empty()) {
    cerr<<"NewOrcaTask::split() Error: cannot create jobs - no jobs to create"<<endl;
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
