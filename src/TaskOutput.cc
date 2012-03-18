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
#include <sstream>
#include "Retrieve.hh"

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
class activeStatus { //Functor for sorting through job vector and finding active jobs
private:
  BossIf* pBossIf_;
public:
  activeStatus(BossIf* myBossIf) : pBossIf_(myBossIf) {};
  bool operator()(Job* pJob) const {
    string status=pBossIf_->status(pJob);
    if((status=="R") || (status=="I") || (status=="T"))  {return false;}
    //any other running states?
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
  if(!task_) return EXIT_FAILURE; //check ptr not empty
  if(!*task_) return EXIT_FAILURE; //check object initialised ok
  if(task_->split()) return EXIT_FAILURE;
  
  //Create Jobs vector for subrange of required jobs
  copy((task_->jobs())->begin(), (task_->jobs())->end(), back_inserter(allJobs_)); //Make a copy of all jobs for task
  allJobs_.erase(remove_if(allJobs_.begin(), allJobs_.end(), JobRange(minJob_, maxJob_)), 
		 allJobs_.end()); //Erase jobs not within jobrange

  
  //used later in auto retrieve
  //taskId_ = myTaskId;
  //jobsNotRetrieved = 0;

  
  //Make a collection containing only finished (status "E") jobs
  /*copy(allJobs_.begin(), allJobs_.end(), back_inserter(finJobs_));
  BossIf myBossIf(task_);  
  finJobs_.erase(remove_if(finJobs_.begin(), finJobs_.end(), 
			   finStatus(&myBossIf)), finJobs_.end());

  for(vector<Job*>::const_iterator it = finJobs_.begin(); it != finJobs_.end() ; it++)
    if(Log::level()>0) cout << "TaskOutput::init() GROSS Id "
			    << (*it)->Id() << " has BOSS status " 
			    << (myBossIf.status(*it))<<endl;

  if(finJobs_.empty()) {
    cout <<"TaskOutput::init : No output available - no jobs found with finished status!"<<endl;
  }
*/
  return EXIT_SUCCESS;
}
int TaskOutput::setFinJobs() {
  copy(allJobs_.begin(), allJobs_.end(), back_inserter(finJobs_));
  BossIf myBossIf(task_);
  Range jobRange(minJob_, maxJob_);
  if(myBossIf.setJobs(jobRange)) return EXIT_FAILURE;
  myBossIf.rangeStatus(jobRange);
  finJobs_.erase(remove_if(finJobs_.begin(), finJobs_.end(),
                           finStatus(&myBossIf)), finJobs_.end());

  for(vector<Job*>::const_iterator it = finJobs_.begin(); it != finJobs_.end() ; it++)
    if(Log::level()>0) cout << "TaskOutput::SetFinJobs() GROSS Id "
                            << (*it)->Id() << " has BOSS status "
                            << (myBossIf.status(*it))<<endl;

  if(finJobs_.empty()) {
    cout <<"TaskOutput::SetFinJobs : No output available - no jobs found with finished status!"<<endl;
  }

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

  for(vector<Job*>::const_iterator it = allJobs_.begin(); it!=allJobs_.end(); ++it) {
    string mySboxDir = getSbox(*it, oDir);
    if(mySboxDir.empty()) {
      mySboxDir = "_SBOX_RETRIEVAL_ERROR_";
      //jobsNotRetrieved++;
    }

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
  if(sboxDir.empty()&&TaskFactory::facType()=="OrcaG") sboxDir=getGridSbox(pJob, oDir); //.. or from Grid
  else if(sboxDir.empty()&&TaskFactory::facType()=="OrcaLoc") sboxDir=getLocalSbox(pJob, oDir); //.. or from local farm
  
  return sboxDir;
}
string TaskOutput::getGridSbox(Job* pJob, string oDir /* ="./" */) const {
  if(!pJob||!task_) {
    report_error("TaskOutput::getGridSbox","TaskOutput not initialised"); 
    return "";
  }

  if(Log::level()>2) cout <<"TaskOutput::getGridSbox attempting to retrieve Sbox from Grid"<<endl;
  BossIf myBossIf(task_);
  int bossId = myBossIf.bossId(pJob);
  if(bossId==0) return "";
  
  //Retrieve via boss-edg command - first must change path in boss table 
  // tells it where to put output
  ostringstream select;
  select<<"ID="<<bossId;
  if(LocalDb::instance()->tableUpdate("JOB", "S_PATH", string("\"")+oDir+string("\""), select.str())) return "";
  EDGIf myEDG;
  string sboxDir = myEDG.getOutput(bossId,oDir);
  if(sboxDir.empty()) return "";
  		  
  //Save result to Db
  saveSboxDir(pJob, sboxDir);
  if (myBossIf.recoverJob(pJob, sboxDir)) return "";
  return sboxDir;
  //check for correct saving of output files to grid 
  //checkOutputLFNs(pJob, sboxDir);
}
string TaskOutput::getLocalSbox(Job* pJob, const string oDir) const {
  BossIf myBossIf(task_);
  int bossId = myBossIf.bossId(pJob);
  EDGIf myEDG;
  string sboxDir = myEDG.untarLocalOutput(bossId,oDir);
  if(sboxDir.empty()) return "";
  //save result to db
  saveSboxDir(pJob, sboxDir);
  if (myBossIf.recoverJob(pJob, oDir)) return "";
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
  select<<"TaskID="<<task_->Id()<<"&&JobID="<<pJob->Id();
  string convS = LocalDb::instance()->escapeString(sboxDir);
  if(convS.empty()) return EXIT_FAILURE;
  if(LocalDb::instance()->tableUpdate("Analy_Job", "SboxDir", convS, select.str())) 
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
int TaskOutput::setAuto(string dir) {
  //if (checkJobsLeft()==0) {
  //  return EXIT_SUCCESS;
  //} else {
    if(createRetrieveScript(dir)) return EXIT_FAILURE;
    ostringstream command;
    command  << "nohup " << dir << "/autoRetrieve.sh " << task_->Id() << " " << dir << " >/dev/null &";
    cout << "Starting autoretrieve for task " << task_->Id() << endl;
    if (Log::level()>2) cout << "starting auto retrieve using command " << command.str().c_str() <<endl;
    if (system(command.str().c_str())) {
      cerr << "Error: unable to start autoretrieve process - please run output command manually"<<endl;
      return EXIT_FAILURE;
    }
  //}
  return EXIT_SUCCESS;
}
int TaskOutput::cancelAuto() { 
  ostringstream command;
  cout << "Stoping auto retrieve of task " << task_->Id() << endl;
  command << "kill -9 `ps -uw | grep \"bash\" | grep \"autoRetrieve.sh " << task_->Id() <<  "\" | cut -d\" \" -f3`";
  if (Log::level()>2) cout << "TaskOutput::Killing auto retieval with command: " << command.str() <<endl;
  if(system(command.str().c_str())==0) {
      cout << "Error: Unable to stop autoretrieve process - please kill manually"<<endl;
  }	      
  return EXIT_SUCCESS;
}
int TaskOutput::checkJobsLeft() {
  copy(allJobs_.begin(), allJobs_.end(), back_inserter(activeJobs_));
  BossIf myBossIf(task_);
  activeJobs_.erase(remove_if(activeJobs_.begin(), activeJobs_.end(),
			                           activeStatus(&myBossIf)), activeJobs_.end());
  
  int jobsNotRetrieved=0;
  for(vector<Job*>::const_iterator it = finJobs_.begin(); it!=finJobs_.end(); ++it) {
    string sandbox=getDbSbox(*it);
    if (sandbox=="") jobsNotRetrieved++;
  }
  return (activeJobs_.size() + jobsNotRetrieved);
}
int TaskOutput::cont() {
  int numjobs_=checkJobsLeft();
  /*if (numjobs_==0) {
    cout << "All jobs retrieved - stopping autoRetrieve" <<endl;
    if(cancelAuto()) return -1;
    return EXIT_SUCCESS;
  }*/
  return numjobs_;
}
int TaskOutput::createRetrieveScript(string dir) {
  string fileName=string("autoRetrieve.sh");
  if(Log::level()>2) cout <<"TaskOutput::getting retrieve script from database"<<endl;
  Retrieve script(fileName, TaskFactory::instance()->facType());
  if(script.save(dir)) return EXIT_FAILURE;
  if(Log::level()>2) cout <<"TaskOutput::script saved to "<<dir<<endl;
  return EXIT_SUCCESS;
}
/*int TaskOutput::recoverJobs(const string journalDir) {
  if(Log::level()>2) cout << "TaskOutput::recoverJobs() checking to see if any jobs need database recovery" <<endl;
  BossIf myBossIf(task_);
  for(vector<Job*>::const_iterator it = finJobs_.begin(); it!=finJobs_.end(); ++it) {
    if(myBossIf.recoverJob((*it), journalDir)) return EXIT_FAILURE;
  }
*/
