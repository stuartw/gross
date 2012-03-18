#include "BossIf.hh"

#include "Error.hh"
#include "Job.hh"
#include "Task.hh"
#include "JDL.hh"
#include "LocalDb.hh"
#include <BossCommandInterpreter.h>

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
  ostringstream bossOutput;
  cout.rdbuf(bossOutput.rdbuf());
  cout <<"===Begin BOSS API submit output:"<<endl;
  if(ci.acceptCommand(bossargc, bossArgs)) return EXIT_FAILURE;
  cout <<"===End BOSS API submit output.==="<<endl;
  cout.rdbuf(psbuf_orig);
  if(Log::level()>0)  cout <<"BossIf::submitJob() Full output from BOSS submission is:"<<endl << bossOutput.str();

  //Get BossID (Not pretty, but only way I can think of doing this...)
  bossId=LocalDb::instance()->maxCol("JOB","ID");
  if(!bossId||bossId!=(lastId+1)) {
    cerr<<"BossIf::SubmitJob() Error cannot get bossId from Db"<<
      " last ID: " << lastId <<" this ID: " << bossId<<endl;
    return EXIT_FAILURE;
  }

  //Now save this submitted job
  if(Log::level()>2) cout <<"BossIf::submitJob() Saving BossId to DB" <<endl;
  if(this->saveId(pTask_->Id(), pJob->Id(), bossId)) return EXIT_FAILURE;

  cout <<"Job with GROSS Id "<<pJob->Id() 
       << " sucessfully submitted with BOSS Id = "<<bossId
       << " and scheduler ID " << this->schedId(pJob)<<endl;

  return EXIT_SUCCESS;
}
const string BossIf::status(const Job* pJob) {
  if(!pTask_) {
    report_error("BossIf::status()","Task not defined");
    return "U";
  }

  if(!this->bossId(pJob)) return "N"; //Not submitted (no bossId)

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
  os<<this->bossId(pJob);
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

  std::istringstream is(f1);
  int readJobId;
  if(!(is>>readJobId)) {
    report_error("BossIf::status()", "In reading id");
    return "U";
  }

  if(!readJobId==(this->bossId(pJob))) { 
    report_error("BossIf::status()", "Queried BOSS ID does not match");
    return "U";
  }
  
  if(Log::level()>2)  cout << "BossIf::status(): GROSS Id "<< pJob->Id() 
			   << " with BOSS Id " << (this->bossId(pJob)) 
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

const string BossIf::queryBossDb(const Job* pJob, const string tablename, 
				 const string field) const { 
  //Note NULL returned for easy printout (this command usually called by user generic query).
  string noVal("NULL");
  if(!pJob || !pTask_) { 
    cerr<<"Job::queryBossDb() Error Job/Task undefined !"<<endl;
    return noVal;
  }
  int bossId = this->bossId(pJob);
  if(!bossId) {
    return noVal;
  }

  //Choose Foreign Key name (BOSS DB SCHEMA DEPENDENT!)
  string fKey;
  if(tablename=="JOB") fKey="ID";
  else fKey="JOBID";
  
  ostringstream os;
  os << fKey << "=" << bossId;
  vector<string> myResults;
  if(LocalDb::instance()->tableRead(tablename, field, os.str(), myResults)) return noVal;
  if(myResults.size()>1) {
    cerr<<"BossIf::queryBossDb() Error reading boss info from Db - multiple values returned"<<endl;
    return noVal;
  }
  return myResults[0];
}

