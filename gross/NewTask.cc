#include "NewTask.hh"

#include "TaskFactory.hh"
#include "PhysCat.hh"
#include "CladLookup.hh"
#include "Job.hh"
#include "File.hh"

//Explicitly inherit from Task base class (safest way to do this!)
int NewTask::save() { return Task::save();};
int NewTask::saveJobs() {return Task::saveJobs();};
int NewTask::createSubFiles(const string& aType) {return Task::createSubFiles(aType);};

int NewTask::init(const File* myUserSpecFile, const int&){
  if(!myUserSpecFile->exists()) {
    cerr <<"NewTask::init(): Error: user spec file does not exist!"<<endl;
    return EXIT_FAILURE;
  }
  if(Log::level()>2) cout<<"NewTask::init(): creating new userSpec from user specified file "
			 << myUserSpecFile->fullHandle()<< endl;
  userClad(myUserSpecFile->contents());
  userSpec(new CladLookup(userClad())); //deleted in ~Task
  return EXIT_SUCCESS;
}
int NewTask::prepareJobs(const string& myType){
  if(Log::level()>2) cout <<"NewTask::prepareJobs() Initialising new Jobs"<<endl;
  if(this->createJobs()) return EXIT_FAILURE;
  if(this->initJobs()) return EXIT_FAILURE;
  if(Log::level()>2) cout <<"NewTask::prepareJobs() Creating Submission files"<<endl;
  if(this->createSubFiles(myType)) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
int NewTask::createJobs(){
  //Create and initialise appropriate PhysCat
  physCat(new PhysCat());  //deleted in ~Task  
  const string sCat = userSpec()->read("RLSCat");
  if(sCat.empty()) {
    cerr<<"NewTask::createJobs() Error RLS Catalogue not defined in JDL (RLSCat=?)"<<endl;
    return EXIT_FAILURE;
  }

  //Construct appropriate meta-data query
  const string sOwner = userSpec()->read("Owner");
  const string sDataset = userSpec()->read("Dataset");
  if(sOwner.empty()||sDataset.empty()) {
    cerr <<"NewTask::createJobs() Error: Dataset Owner and/or Dataset name not defined in JDL (Owner=?/Dataset=?)"<<endl;
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
    jobs_.push_back((TaskFactory::instance())->makeJob("NEW", jobId, (*i), this));   //deleted in ~Task
    jobId++;
  }
  if(jobs_.empty()) {
    cerr<<"NewTask::createJobs() Error: cannot create jobs"<<endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
int NewTask::initJobs(){
  if(jobs_.empty()) {
    cerr<<"NewTask::initJobs() Error: no jobs defined"<<endl;
    return EXIT_FAILURE;
  }
  for(vector<Job*>::const_iterator i = jobs_.begin(); i!=jobs_.end(); i++) {
    if(Log::level()>2) cout <<"NewTask::initJobs() Initialising job with id " << (*i)->Id() <<endl;
    if((*i)->init()) return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
