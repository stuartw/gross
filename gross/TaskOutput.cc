#include "TaskOutput.hh"

#include "Task.hh"
#include "Job.hh"
#include "TaskFactory.hh"
#include "File.hh"
#include "BossIf.hh"
#include "EDGIf.hh"
#include "Error.hh"
#include "LocalDb.hh"
#include <algorithm>
#include <iomanip>
#include "Range.hh"

class finStatus { //Functor for sorting through job vector and finding finished jobs
private:
  BossIf* pBossIf_;
public:
  finStatus(BossIf* myBossIf) : pBossIf_(myBossIf) {};
  bool operator()(Job* pJob) const {
    if((pBossIf_->status(pJob))=="E") {return false;} 
    else return true;
  }
};

TaskOutput::TaskOutput() : task_(0), minJob_(0), maxJob_(0)  {};

int TaskOutput::init(int myTaskId, int minJobId /*=0*/, int maxJobId /*=0*/) {
  if(!myTaskId) return EXIT_FAILURE;

  minJob_=minJobId;
  maxJob_=maxJobId;

  //Create and initialise task
  task_=(TaskFactory::instance())->makeTask(myTaskId);
  if(!task_) return EXIT_FAILURE;
  File nullFile("NULL");
  if(task_->init(&nullFile, myTaskId)) return EXIT_FAILURE;
  if(task_->queryPrepareJobs()) return EXIT_FAILURE;

  //Create Jobs vector for subrange of required jobs
  copy((task_->jobs())->begin(), (task_->jobs())->end(), back_inserter(allJobs_)); //Make a copy of all jobs for task
  allJobs_.erase(remove_if(allJobs_.begin(), allJobs_.end(), JobRange(minJob_, maxJob_)), 
		 allJobs_.end()); //Erase jobs not within jobrange


  //Make a collection containing only finished (status "E") jobs
  copy(allJobs_.begin(), allJobs_.end(), back_inserter(finJobs_));
  BossIf myBossIf(task_);  
  finJobs_.erase(remove_if(finJobs_.begin(), finJobs_.end(), 
			   finStatus(&myBossIf)), finJobs_.end());

  for(vector<Job*>::const_iterator it = finJobs_.begin(); it != finJobs_.end() ; it++)
    if(Log::level()>0) cout << "TaskOutput::init() GROSS Id "
			    << (*it)->Id() << " has BOSS status " 
			    << (myBossIf.status(*it))<<endl;

  if(finJobs_.empty())
    cout <<"TaskOutput::init : No output available - no jobs found with finished status!"<<endl;
    
  
  return EXIT_SUCCESS;
}
int TaskOutput::printSboxDirs(string oDir /* ="./" */) const {
  if(!task_) { 
    report_error("TaskOutput::printSboxDirs","TaskOutput not initialised");
    return EXIT_FAILURE;
  }
  //Process each finished job

  cout.setf(std::ios::left, std::ios::adjustfield);
  cout 
    <<std::setw(9)  << "Task ID"
    <<std::setw(14) << "GROSS Job ID"
    << "Sandbox Local Output Directory"<<endl;

  for(vector<Job*>::const_iterator it = finJobs_.begin(); it!=finJobs_.end(); ++it) {
    string mySboxDir = getSbox(*it, oDir);
    if(mySboxDir.empty()) mySboxDir = "_SBOX_RETRIEVAL_ERROR_";
    cout << std::setw(9) << task_->Id() 
	 << std::setw(14) << (*it)->Id() 
	 << mySboxDir <<endl;
  }
  return EXIT_SUCCESS;
}
string TaskOutput::getSbox(Job* pJob, string oDir /* ="./" */) const {
  string sboxDir("");

  if(!task_||!pJob) {
    report_error("TaskOutput::getSbox","TaskOutput not initialised"); 
    return sboxDir;
  }  
  
  //Ensure job has status finished
  vector<Job*>::const_iterator pos = find(finJobs_.begin(), finJobs_.end(), pJob);
  if(pos==finJobs_.end()) return sboxDir;

  sboxDir = getDbSbox(pJob); //retrieve from Db
  if(sboxDir.empty()) sboxDir=getGridSbox(pJob, oDir); //.. or from Grid
  
  return sboxDir;
}
string TaskOutput::getGridSbox(Job* pJob, string oDir /* ="./" */) const {
  if(!pJob||!task_) {
    report_error("TaskOutput::getGridSbox","TaskOutput not initialised"); 
    return "";
  }

  if(Log::level()>2) cout <<"TaskOutput::getGridSbox attempting to retrieve Sbox from Grid"<<endl;
  BossIf myBossIf(task_);
  string edgId = myBossIf.schedId(pJob);
  if(edgId.empty()) return "";
  //Retrieve via edg command
  EDGIf myEDG;
  string sboxDir = myEDG.getOutput(edgId, oDir);
  if(sboxDir.empty()) return "";
		  
  //Save result to Db
  this->saveSboxDir(pJob, sboxDir);
  return sboxDir;
}
string TaskOutput::getDbSbox(Job* pJob) const{
  if(!task_||!pJob) { 
    report_error("TaskOutput::getDbSbox","TaskOutput not initialised"); 
    return "";
  }

  if(Log::level()>2) cout <<"TaskOutput::getDbSbox looking up Sbox local dir on Db"<<endl;
  ostringstream qry;
  qry << "TaskID="<<task_->Id() <<"&&JobID="<<pJob->Id();
  vector<string> myResults;
  if(LocalDb::instance()->tableRead("Analy_Job","SboxDir", qry.str(), myResults)) return "";
  if(myResults.size()!=1) {
    cerr<<"TaskOutput::getDbSbox Error reading SboxDir from Db"<<endl;
    return "";
  }
  if(myResults[0].empty() || myResults[0]=="NULL") return "";
  return myResults[0];
}
int TaskOutput::saveSboxDir(Job* pJob, string sboxDir) const {
  if(!task_||!pJob) {
    report_error("TaskOutput::saveSboxDir","TaskOutput not initialised");
    return EXIT_FAILURE;
  }
  ostringstream select;
  select<<"TaskID="<<this->task_->Id()<<"&&JobID="<<pJob->Id();
  string convS = LocalDb::instance()->escapeString(sboxDir);
  if(convS.empty()) return EXIT_FAILURE;
  if(LocalDb::instance()->tableUpdate("Analy_Job", "SboxDir", convS, select.str())) 
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}