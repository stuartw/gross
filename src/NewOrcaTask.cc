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
  
  //Initialise catalogue - find all runs
  physCat(new PhysCat(sOwner, sDataset, sDataChain));  //deleted in ~NewOrcaTask
  vector<int> runs_ = physCat()->getAllRuns();
  if (runs_.empty()) {
    cerr<<"NewOrcaTask::split() Error: cannot create jobs - no runs found for owner=" << sOwner << " Dataset=" << sDataset <<endl;
    return EXIT_FAILURE;
  }    
  
  int runsPerJob = atoi((userSpec()->read("RunsPerJob")).c_str());
  int FirstRun = atoi((userSpec()->read("FirstRun")).c_str());
  int LastRun = atoi((userSpec()->read("LastRun")).c_str());
  if (runsPerJob<=0) {
    if(Log::level()>0) cout<< "NewOrcaTask::split() Warning: RunsPerJob not correctly defined - set to default value (1)"<<endl;
    runsPerJob=1;
  }
  if (FirstRun<=0||count(runs_.begin(), runs_.end(),FirstRun)==0) {
    cout<< "NewOrcaTask::split() Warning: FirstRun (" << FirstRun << ") not in list from RefDB - set to " << runs_.front() <<endl;
    FirstRun=runs_.front();
  }
  if (LastRun<=0||count(runs_.begin(), runs_.end(),LastRun)==0) {
    cout<< "NewOrcaTask::split() Warning: LastRun (" << LastRun << ") not in list from RefDB - set to "<< runs_.back() << endl;
    LastRun=runs_.back();
  }
  if (FirstRun>LastRun) {
    cerr << "NewOrcaTask::split() Error: FirstRun is a larger number than LastRun" <<endl;
    return EXIT_FAILURE;
  }
    
  //get all runs - then keep only the ones needed, assume a sorted vector.
  vector<int>::const_iterator first = find(runs_.begin(), runs_.end(), FirstRun);
  vector<int>::const_iterator last = find(runs_.begin(), runs_.end(), LastRun);
  vector<int> runs2_(first, ++last);  
  
  int jobId=1;
  for(vector<int>::const_iterator i = runs2_.begin(); i<runs2_.end()  ; i+= runsPerJob) {
    //create sub vector of runs specific to that job - if not enough runs left then make job from however many are left
    vector<int> myRuns; //must be visible outside if loop below
    if (runs2_.end()>i+runsPerJob) myRuns = vector<int>(i, i+runsPerJob);
    else  myRuns = vector<int>(i, i+(runs2_.end()-i));
  
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
