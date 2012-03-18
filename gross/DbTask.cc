#include "DbTask.hh"

#include "BossIf.hh"
#include "TaskFactory.hh"
#include "Job.hh"
#include "CladLookup.hh"
#include "File.hh"
#include "LocalDb.hh"

//Explicitly inherit from Task base class (safest way to do this!)
int DbTask::save() {return Task::save();};
int DbTask::saveJobs() {return Task::saveJobs();};
int DbTask::createSubFiles(const string& aType) {return Task::createSubFiles(aType);};

int DbTask::queryPrepareJobs() {
  if(Log::level()>2) cout <<"DbTask::queryPrepareJobs() Initialising Jobs from db"<<endl;
  //Minimal job creation
  if(this->createJobs()) return EXIT_FAILURE;
  if(this->initJobs()) return EXIT_FAILURE;
  
  if(jobs_.empty()) {
    cerr<<"DbTask::queryPrepareJobs() Error: no jobs defined"<<endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int DbTask::prepareJobs(const string& myType) {
  if(Log::level()>2) cout <<"DbTask::prepareJobs() Initialising Jobs from db"<<endl;
  if(this->createJobs()) return EXIT_FAILURE;
  if(this->initJobs()) return EXIT_FAILURE;
  
  if(Log::level()>2) cout <<"DbTask::prepareJobs() Setting JDLRem from db"<<endl;
  if(this->setJdlRem()) return EXIT_FAILURE; //Get JDLRem from Db and set userSpec with it (prior to sub. file creation)

  if(Log::level()>2) cout <<"DbTask::prepareJobs() Creating Submission files"<<endl;
  if(this->createSubFiles(myType)) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
int DbTask::createJobs() {
  if(!this->Id()) {
    cerr<<"DbTask::createJobs() Error: Task has no ID"<<endl;
    return EXIT_FAILURE; //Task must be on database first
  }
  ostringstream os;
  os<< "TaskID="<<this->Id();
  vector<string> resDataSel, resJobId;
  if(LocalDb::instance()->tableRead("Analy_Job", "DataSelect", os.str(), resDataSel)) return EXIT_FAILURE;

  //Get from Database DataSelect and JobId for particular Task to create new job with
  for(vector<string>::const_iterator i = resDataSel.begin(); i!=resDataSel.end(); i++) {
    os.str("");
    os<< "TaskID="<<this->Id()<<"&&DataSelect="<<(*i);
    resJobId.clear();
    if(LocalDb::instance()->tableRead("Analy_Job", "JobID", os.str(), resJobId)) return EXIT_FAILURE;
    if(resJobId.size()>1) {
      cerr<<"DbTask::createJobs() Error : found more than one job matching dataselection"<<endl;
      return EXIT_FAILURE;
    }	
    for(vector<string>::const_iterator j = resJobId.begin(); j!=resJobId.end(); j++) {
      if(Log::level()>0) cout <<"DbTask::createJobs() Creating job with id " << (*j) << " for data selection (run) " << (*i)<<endl;
      jobs_.push_back((TaskFactory::instance())->makeJob("DB", atoi((*j).c_str()), atoi((*i).c_str()), this));   //deleted in ~Task
    }
  }
  if(jobs_.empty()) {
    cerr<<"DbTask::createJobs() Error: cannot create jobs for task "<< this->Id() << endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
int DbTask::initJobs(){
  if(jobs_.empty()) {
    cerr<<"DbTask::initJobs() Error: no jobs defined"<<endl;
    return EXIT_FAILURE;
  }
  for(vector<Job*>::const_iterator i = jobs_.begin(); i!=jobs_.end(); i++) {
    if(Log::level()>2) cout <<"DbTask::createJobs() Initialising job with id " << (*i)->Id() <<endl;
    if((*i)->init()) return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
int DbTask::init(const File*, const int& myId){
  if(!myId) return EXIT_FAILURE;
  if(Log::level()>2) cout<<"DbTask::init(): creating new userSpec from database with id "<< myId <<endl;
  
  ostringstream os;
  os << "ID="<<myId;
  vector<string> myResults;
  LocalDb::instance()->tableRead("Analy_Task", "UserSpec", os.str(), myResults);
  if(myResults.size()!=1) {
    cerr<<"DbTask::init() Error reading UserSpec from Db"<<endl;
    return EXIT_FAILURE;
  }
  if(!myResults[0].empty()) userClad(myResults[0]);
  else {
    cerr<<"DbTask::init Error - UserSpec not defined in DB"<<endl;
    return EXIT_FAILURE;
  }

  userSpec(new CladLookup(userClad())); //deleted in ~Task
  Id(myId); //setting ID value shows that Task does not require saving again
  
  return EXIT_SUCCESS;
}
int DbTask::setJdlRem() {
  //This needs to be taken from Db and set within userSpec as the JDL object uses 
  //the CladLookup jdlDump method to create the JDL file for a job (it does not read
  //it from the Db itself).
  if(!this->Id() || !this->userSpec()) {
    cerr<<"DbTask::setJdlRem() Error Task not initialised properly"<<endl;
    return EXIT_FAILURE;
  }
  ostringstream os;
  os << "ID="<<this->Id();
  
  vector<string> myResults;
  LocalDb::instance()->tableRead("Analy_Task", "JDLRem", os.str(), myResults);
  if(myResults.size()!=1) {
    cerr<<"DbTask::setJdlRem() Error reading JdlRem from Db"<<endl;
    return EXIT_FAILURE;
  }
  userSpec()->jdlDump(myResults[0]);
  
  return EXIT_SUCCESS;
}
