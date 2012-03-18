#include "BossIf.hh"

#include "Error.hh"
#include "Job.hh"
#include "Task.hh"
#include "JDL.hh"
#include "LocalDb.hh"
#include <BossCommandInterpreter.h>
#include "Range.hh"
#include "StringSpecial.hh"

BossIf::BossIf(const Task* pTask) : pTask_(pTask) {}

int BossIf::submitJob(const string mySched, const string myBossJobType, const Job* pJob) const{
  int bossId = 0;
  if(!pTask_) {
    cerr<<"BossIf::submitJob() Error Task not defined!"<<endl;
    return EXIT_FAILURE;
  }
  if(!pJob) {
    cerr<<"BossIf::submitJob() Error undefined Job"<<endl;
    return EXIT_FAILURE;
  }
  BossCommandInterpreter ci;
  //Set up arguments for BossCommandInterpreter
  int bossargc=7;
  char* bossArgs[bossargc];
  bossArgs[0] = "submit";
  bossArgs[1] = "-scheduler";
  bossArgs[2] = const_cast<char*> (mySched.c_str());
  bossArgs[3] = "-jobtype";
  bossArgs[4] = const_cast<char*> (myBossJobType.c_str());
  bossArgs[5] = "-classad";
  const char* buff = ((pJob->jdl())->fullHandle()).c_str();
  bossArgs[6] = const_cast<char*> (buff);
  if(Log::level()>0) {
    cout << "BossIf::SubmitJob() Submitting to boss with arguments: "<<endl;
    for(int i=0; i<bossargc;i++)
      cout << bossArgs[i]<< " ";
    cout <<endl;
  }
  //Get latest BossID before and after submission
  int lastId=LocalDb::instance()->maxCol("JOB","ID");
  //Redirect cout to an ostringstream and run Boss command
  std::streambuf* psbuf_orig = cout.rdbuf();
  std::streambuf* cerr_orig = cerr.rdbuf();
  ostringstream bossOutput;
  cout.rdbuf(bossOutput.rdbuf());
  cerr.rdbuf(bossOutput.rdbuf());
  cout <<"===Begin BOSS API submit output:"<<endl;
  if(ci.acceptCommand(bossargc, bossArgs)) {
    cout.rdbuf(psbuf_orig);
    cerr.rdbuf(cerr_orig);
    cerr << "BossIf::submitJob Error Job not submitted - see comand output below" <<endl;
    cerr << bossOutput.str() <<endl<<endl;	  
    return EXIT_FAILURE;
  }
  cout <<"===End BOSS API submit output.==="<<endl;
  cout.rdbuf(psbuf_orig);
  cerr.rdbuf(cerr_orig);
  if(Log::level()>0) cout <<"BossIf::submitJob() Full output from BOSS submission is:"<<endl << bossOutput.str();
  
  //Get BossID (Not pretty, but only way I can think of doing this...)
  bossId=LocalDb::instance()->maxCol("JOB","ID");
  if(!bossId||bossId!=(lastId+1)) {
    cerr<<"BossIf::SubmitJob() Error cannot get bossId from Db"<<
      " last ID: " << lastId <<" this ID: " << bossId<<endl;
    return EXIT_FAILURE;
  }

  //Now save this submitted job
  if(Log::level()>2) cout <<"BossIf::submitJob() Saving BossId to DB" <<endl;
  if(saveId(pTask_->Id(), pJob->Id(), bossId)) return EXIT_FAILURE;

    //check scheduler id returned if not error;
  if (schedId(pJob)=="") {
  cout <<"BossIf::submitJob() Full output from BOSS submission is:"<<endl << bossOutput.str();
  cerr << "Job not submitted - will not submit any more jobs" << endl;
    return EXIT_FAILURE;
  }
  
  cout <<"Job with GROSS Id "<<pJob->Id() 
       << " sucessfully submitted with BOSS Id = "<<bossId
       << " and scheduler ID " << schedId(pJob)<<endl;

  return EXIT_SUCCESS;
}
const string BossIf::status(const Job* pJob) {
  if(!pTask_) {
    report_error("BossIf::status()","Task not defined");
    return "U";
  }

  if(!bossId(pJob)) return "N"; //Not submitted (no bossId)

  //Check if status already exists
  for(IntStringMap::const_iterator pos = jobStatusMap_.begin(); pos != jobStatusMap_.end() ; ++pos)
    if((pos->first) == (pJob->Id())) return (pos->second);
  
  //Setup Boss commands
  BossCommandInterpreter ci;
  int bossargc=4;
  char* bossArgs[bossargc];
  bossArgs[0] = "query";
  bossArgs[1] = "-jobid";
  ostringstream os;
  os<<bossId(pJob);
  bossArgs[2] = const_cast<char*> ((os.str()).c_str());
  bossArgs[3] = "-statusOnly";
  if(Log::level()>2)  {
    cout << "BossIf::status() Querying boss job. Boss arguments are: "<<endl;
    for(int i=0; i<bossargc;i++)
      cout << bossArgs[i]<< " ";
    cout <<endl;
  }
  //Redirect cout to an ostringstream and run Boss query
  std::streambuf* psbuf_orig = cout.rdbuf();
  ostringstream bossOutput;
  cout.rdbuf(bossOutput.rdbuf());
  if(ci.acceptCommand(bossargc, bossArgs)) {
    cout.rdbuf(psbuf_orig);
    cerr<<"BossIf::status() Warning: BOSS query failure"<<endl;
    return "U";
  }
  cout.rdbuf(psbuf_orig);
  if(Log::level()>2)  cout <<"BossIf::status() The boss result is:"<<endl << bossOutput.str();
  
  //Process Output in a safe way
  if((bossOutput.str()).empty()) return "U";
  string::size_type endf1 = (bossOutput.str()).find_first_of(" \t\n");
  string f1= (bossOutput.str()).substr(0,endf1);
  string status = (bossOutput.str()).substr(endf1+1,1);
  if (status=="M") status="E"; //dont know what M is
  std::istringstream is(f1);
  int readJobId;
  if(!(is>>readJobId)) {
    report_error("BossIf::status()", "In reading id");
    return "U";
  }

  if(!readJobId==(bossId(pJob))) { 
    report_error("BossIf::status()", "Queried BOSS ID does not match");
    return "U";
  }
  
  if(Log::level()>2)  cout << "BossIf::status(): GROSS Id "<< pJob->Id() 
			   << " with BOSS Id " << (bossId(pJob)) 
			   << " has status " << status<<endl;
  jobStatusMap_[(pJob->Id())] = status;
  return status;
}
int BossIf::submitTask(const string mySched, const string myBossType) const {
  if(!pTask_) {
    cerr<<"BossIf::submitTask() Error Task not defined!"<<endl;
    return EXIT_FAILURE;
  }
  const vector<Job*>* pJobs = pTask_->jobs();
  if(pJobs->empty()) {    
    cerr<<"BossIf::submitTask() Error: no jobs defined for Task"<<endl;
    return EXIT_FAILURE;
  }

  for(vector<Job*>::const_iterator i = pJobs->begin(); i!=pJobs->end(); i++) {
    if(Log::level()>2) cout <<"BossIf::submitTask() Submitting job with id " << (*i)->Id() <<endl;
    if(submitJob(mySched, myBossType, (*i))) return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
int BossIf::submitJobs(const string mySched, const string myBossType, int minJobId, int maxJobId) {
  if(!pTask_) {
    cerr<<"BossIf::submitJobs() Error Task not defined!"<<endl;
    return EXIT_FAILURE;
  }

  minJob_=minJobId;
  maxJob_=maxJobId;

  copy((pTask_->jobs())->begin(), (pTask_->jobs())->end(), back_inserter(jobs_)); //Make a copy of all jobs for task
  jobs_.erase(remove_if(jobs_.begin(), jobs_.end(), JobRange(minJob_, maxJob_)), jobs_.end()); //Erase jobs not within jobrange
   
  if(jobs_.empty()) {
    cerr<<"BossIf::submitJobs() Error: no jobs in range"<<endl;
    return EXIT_FAILURE;
  }	

  for(vector<Job*>::const_iterator i = jobs_.begin(); i!=jobs_.end(); i++) {
    if(Log::level()>2) cout <<"BossIf::submitJobs() Submitting job with id " << (*i)->Id() <<endl;
    if(submitJob(mySched, myBossType, (*i))) return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;    
}	
int BossIf::saveId(const int myTaskId, const int myJobId, const int myBossId) const {
  if(!myTaskId || !myJobId || !myBossId) {
    cerr<<"BossIf::saveId() Error undefined ID!"<<endl;
    return EXIT_FAILURE;
  }
  ostringstream os;
  os << "TaskID="<<myTaskId <<"&&JobID="<<myJobId<<endl; 
  ostringstream sbossId;
  sbossId<<myBossId;
  if(Log::level()>2) cout<<"BossIf::saveId() Saving bossId "<<myBossId<<endl;  
  if(LocalDb::instance()->tableUpdate("Analy_Job","BossID",sbossId.str(), os.str())) return EXIT_FAILURE;

  //New BossId being saved - this is time to ensure SboxDir is null (would not be if this was a resubmission)
  if(Log::level()>2) cout<<"BossIf::saveId() Clearing SboxDir "<<endl;  
  if(LocalDb::instance()->tableUpdate("Analy_Job","SboxDir", "NULL" , os.str())) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
int BossIf::bossId(const Job* pJob) const {
  int bossId=0;
  if(!pJob || !pTask_) { 
    cerr<<"Job::bossId() Error Job/Task undefined!"<<endl;
    return bossId;
  }
  ostringstream os;
  os << "TaskID="<<pTask_->Id() <<"&&JobID="<<pJob->Id()<<endl;
  if(Log::level()>2) cout<<"BossIf::bossId() Getting bossId from Db"<<endl;  
  vector<string> myResults;
  if(LocalDb::instance()->tableRead("Analy_Job","BossID",os.str(), myResults)) return bossId;
  if(myResults.size()!=1) {
    cerr<<"BossIf::bossId() Error reading bossID from Db"<<endl;
    return bossId;
  }
  bossId = atoi(myResults[0].c_str());
  return bossId;
}
const string BossIf::schedId(const Job* pJob) const {
  string s = queryBossDb(pJob, "JOB", "SID");
  if(s=="NULL") return "";
  return s;
}
const string BossIf::exitStatus(const Job* pJob) const {
  string s = queryBossDb(pJob, "JOB", "RET_CODE");
  if(s=="NULL") return "";
  return stringSpecial::trim(s);
}
const string BossIf::queryBossDb(const Job* pJob, const string tablename, 
				 const string field) const { 
  //Note NULL returned for easy printout (this command usually called by user generic query).
  string noVal("NULL");
  if(!pJob || !pTask_) { 
    cerr<<"Job::queryBossDb() Error Job/Task undefined !"<<endl;
    return noVal;
  }
  int myBossId = bossId(pJob);
  if(!myBossId) {
    return noVal;
  }

  //Choose Foreign Key name (BOSS DB SCHEMA DEPENDENT!)
  string fKey;
  if(tablename=="JOB") fKey="ID";
  else fKey="JOBID";
  
  ostringstream os;
  os << fKey << "=" << myBossId;
  vector<string> myResults;
  if(LocalDb::instance()->tableRead(tablename, field, os.str(), myResults)) return noVal;
  if(myResults.size()>1) {
    cerr<<"BossIf::queryBossDb() Error reading boss info from Db - multiple values returned"<<endl;
    return noVal;
  }
  return myResults[0];
}

int BossIf::killJob(const int jobId) const {
  ostringstream os;
  os << jobId;
  BossCommandInterpreter ci;
  int bossargc=3;
  char* bossArgs[bossargc]; 
  bossArgs[0] = "kill";
  bossArgs[1] = "-jobid"; 
  bossArgs[2] = const_cast<char*> (os.str().c_str());
  if(Log::level()>0) {
    cout << "BossIf::killJob() killing Boss job with arguments: "<<endl;
    for(int i=0; i<bossargc;i++)
      cout << bossArgs[i]<< " ";
      cout <<endl;
   }

  //Redirect cout to an ostringstream and run Boss command
  std::streambuf* psbuf_orig = cout.rdbuf();
  ostringstream bossOutput;
  cout.rdbuf(bossOutput.rdbuf());
  cout <<"===Begin BOSS API submit output:"<<endl;
  if(ci.acceptCommand(bossargc, bossArgs)) return EXIT_FAILURE;
  cout <<"===End BOSS API submit output.==="<<endl;
  cout.rdbuf(psbuf_orig);
  if(Log::level()>0)  cout <<"BossIf::submitJob() Full output from BOSS kill is:"<<endl << bossOutput.str();
  cout << "Job with BOSS Id " << jobId << " successfully deleted from scheduler"<<endl;
 
  return EXIT_SUCCESS;
}

int BossIf::killTask() const {
  if(!pTask_) {
    cerr<<"BossIf::killTask() Error Task not defined!"<<endl;
    return EXIT_FAILURE;
  }
  const vector<Job*>* pJobs = pTask_->jobs();
  if(pJobs->empty()) {
    cerr<<"BossIf::killTask() Error: no jobs defined for Task"<<endl;
    return EXIT_FAILURE;
  }

  for(vector<Job*>::const_iterator i = pJobs->begin(); i!=pJobs->end(); i++) {
    if(Log::level()>2) cout <<"BossIf::killTask() killing job with id " << (*i)->Id() <<endl;
    if(killJob(bossId(*i))) return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
int BossIf::killJobs(int minJobId, int maxJobId) {
  if(!pTask_) {
    cerr<<"BossIf::killJobs() Error Task not defined!"<<endl;
    return EXIT_FAILURE;
  }

  minJob_=minJobId;
  maxJob_=maxJobId;

  copy((pTask_->jobs())->begin(), (pTask_->jobs())->end(), back_inserter(jobs_)); //Make a copy of all jobs for task
  jobs_.erase(remove_if(jobs_.begin(), jobs_.end(), JobRange(minJob_, maxJob_)), jobs_.end()); //Erase jobs not within jobrange

  if(jobs_.empty()) {
    cerr<<"BossIf::killJobs() Error: no jobs in range"<<endl;
    return EXIT_FAILURE;
  }
  
  for(vector<Job*>::const_iterator i = jobs_.begin(); i!=jobs_.end(); i++) {
    if(Log::level()>2) cout <<"BossIf::KillJobs() Killing job with id " << (*i)->Id() <<endl;
    if(killJob(bossId(*i))) return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
int BossIf::recoverJob(Job* job, const string journalDir) {
  //currently doesn't seem to work
  return EXIT_SUCCESS;
	if (Log::level()>3) cout << "BossIf::recoverJob() checking to see if finished job needs its db entries updated" <<endl;
  vector<string> myResults;
  myResults.push_back(queryBossDb(job, "JOB", "RET_CODE"));
  if (myResults.size()!=1) return EXIT_FAILURE;
  int id = bossId(job);
  if (myResults[0]!="") return EXIT_SUCCESS; 
  else {
    //recover boss jobs
    ostringstream os;
    os << id;
    int bossargc=5;
    char* bossArgs[bossargc];
    bossArgs[0] = "recoverJob";
    bossArgs[1] = "-jobid";
    bossArgs[2] = const_cast<char*> (os.str().c_str());
    bossArgs[3] = "-journaldir";
    bossArgs[4] = const_cast<char*> (journalDir.c_str());
    if(Log::level()>0) {
      cout << "BossIf::recoverJobs() Submitting to boss with arguments: "<<endl;
      for(int i=0; i<bossargc;i++)
      cout << bossArgs[i]<< " ";
      cout <<endl;
    }	    
    //Redirect cout to an ostringstream and run Boss command
    std::streambuf* psbuf_orig = cout.rdbuf();
    std::streambuf* cerr_orig = cerr.rdbuf();
    ostringstream bossOutput;
    cout.rdbuf(bossOutput.rdbuf());
    cerr.rdbuf(bossOutput.rdbuf());
    cout <<"===Begin BOSS API recoverJob output:"<<endl;
    BossCommandInterpreter ci;
    if(ci.acceptCommand(bossargc, bossArgs)) {
      cout <<"===End BOSS API recoverJob output.==="<<endl;
      cout.rdbuf(psbuf_orig);
      cerr.rdbuf(cerr_orig);
      cerr << "BossIf::recoverJobs() Error Job not recovered - see comand output below" <<endl;
      cerr << bossOutput.str() <<endl<<endl;
      return EXIT_FAILURE;
    }
    cout <<"===End BOSS API recoverJob output.==="<<endl;
    cout.rdbuf(psbuf_orig);
    cerr.rdbuf(cerr_orig);
    if(Log::level()>0) cout <<"BossIf::recoverJob Full output from BOSS submission is:"<<endl << bossOutput.str();  
  }
  cout << "BossIf::recoverJob() database entry for job " << id << "has been recreated" << endl;  
  return EXIT_SUCCESS;
}
