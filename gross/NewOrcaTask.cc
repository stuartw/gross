#include "NewOrcaTask.hh"

#include "TaskFactory.hh"
#include "PhysCat.hh"
#include "Job.hh"
#include "CladLookup.hh"

NewOrcaTask::~NewOrcaTask() {
  if(physCat_) delete physCat_;
}

int NewOrcaTask::split(){
  //Create and initialise appropriate PhysCat
  physCat(new PhysCat());  //deleted in ~NewOrcaTask  
  const string sCat = userSpec()->read("RLSCat");
  if(sCat.empty()) {
    cerr<<"NewOrcaTask::split() Error RLS Catalogue not defined in JDL (RLSCat=?)"<<endl;
    return EXIT_FAILURE;
  }

  //Construct appropriate meta-data query
  const string sOwner = userSpec()->read("Owner");
  const string sDataset = userSpec()->read("Dataset");
  if(sOwner.empty()||sDataset.empty()) {
    cerr <<"NewOrcaTask::split() Error: Dataset Owner and/or Dataset name not defined in JDL (Owner=?/Dataset=?)"<<endl;
    return EXIT_FAILURE;
  }
  ostringstream query;
  query << "owner='"<<sOwner<<"' AND dataset='"<<sDataset<<"'";

  //Initialise catalogue - this will create temporary local cat
  if(physCat()->init(sCat, query.str())) return EXIT_FAILURE;

  //Now include user subquery if applicable
  const string subQuery = userSpec()->read("RLSMetaDataQuery");

  vector<int> runs_ = physCat()->runs(subQuery);
  int jobId=1;
  for(vector<int>::const_iterator i = runs_.begin(); i!=runs_.end(); i++) {
    if(Log::level()>-1) cout <<"Creating job with id " << jobId << " for data selection (run) " << (*i)<<endl;
    
    Job* pJ = (TaskFactory::instance())->makeJob(jobId, (*i), this);
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
