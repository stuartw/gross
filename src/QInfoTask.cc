#include "QInfoTask.hh"

#include "Task.hh"
#include "Job.hh"
#include "TaskFactory.hh"
#include "File.hh"
#include "BossIf.hh"
#include "CladLookup.hh"
#include <iomanip>
#include "Range.hh"
#include "ContPrint.hh"

QInfoTask::QInfoTask() : task_(0), minJob_(0), maxJob_(0), header_(true)  {};

QInfoTask::~QInfoTask() {
  if(task_) delete task_;
}

int QInfoTask::init(int myTaskId, int minJobId /*=0*/, int maxJobId /*=0*/) {
  if(!myTaskId) return EXIT_FAILURE;

  minJob_=minJobId;
  maxJob_=maxJobId;
  
  //Create and initialise task
  task_=(TaskFactory::instance())->makeTask(myTaskId);
  if(!task_) return EXIT_FAILURE; //check ptr not empty
  if(!*task_) return EXIT_FAILURE; //check object initialised ok
  if(task_->split()) return EXIT_FAILURE;

  copy((task_->jobs())->begin(), (task_->jobs())->end(), back_inserter(jobs_)); //Make a copy of all jobs for task
  jobs_.erase(remove_if(jobs_.begin(), jobs_.end(), JobRange(minJob_, maxJob_)), jobs_.end()); //Erase jobs not within jobrange
  return EXIT_SUCCESS;
}

  int QInfoTask::printTask() const {
  if(!task_) return EXIT_FAILURE;
  if(header_) cout << "Task Info:"<<endl;
  cout << "User Specification for Task ID "<< task_->Id()<<endl;
  cout << *(task_->userSpec()) <<endl;
  cout << endl;
  return EXIT_SUCCESS;
}

int QInfoTask::printJobs() const {
  if(!task_||jobs_.empty()) {
    cerr<< " QInfoTask::printJobs() Error : no task/jobs defined!"<<endl;
    return EXIT_FAILURE;
  }
  if(header_) cout << "Job info:"<<endl;
  for(vector<Job*>::const_iterator it = jobs_.begin(); it != jobs_.end() ; it++) (*it)->print();
  cout << endl;
  return EXIT_SUCCESS;
}

int QInfoTask::printDataQuery() const {
  if(!task_||jobs_.empty()) {
    cerr<< " QInfoTask::printDataQuery() Error : no task/jobs defined!"<<endl;
    return EXIT_FAILURE;
  }

  //Task Query
  const string sOwner = (task_->userSpec())->read("Owner");
  const string sDataset = (task_->userSpec())->read("Dataset");
  const string sQry = ((task_->userSpec())->read("RLSMetaDataQuery")); 

  //Job Data selection
  cout.setf(std::ios::left, std::ios::adjustfield);
  if(header_) cout 
    <<std::setw(9)  << "Task ID"
    <<std::setw(14) << "GROSS Job ID"
    <<std::setw(30) << "Job data selection"
    <<std::setw(30) << "Dataset Name"
    <<std::setw(30) << "Dataset Owner"
    <<endl;
    for(vector<Job*>::const_iterator it = jobs_.begin(); it != jobs_.end() ; it++)
    cout 
      << std::setw(9)  << task_->Id() 
      << std::setw(14) << (*it)->Id() 
      << std::setw(30) << (*it)->dataSelect()
      << std::setw(30) << sDataset
      << std::setw(30) << sOwner
      <<endl;

  return EXIT_SUCCESS;
}

int QInfoTask::printSumStatus() const {
  if(!task_||jobs_.empty()) {
    cerr<< " QInfoTask::printStatus() Error : no task/jobs defined!"<<endl;
    return EXIT_FAILURE;
  }

  BossIf myBossIf(task_);
  Range jobs(minJob_, maxJob_);
  myBossIf.setJobs(jobs);
  myBossIf.rangeStatus(jobs);
  std::multiset<string> foundStatus;
  std::multiset<string> foundExitCode;
  for (vector<Job*>::const_iterator it = jobs_.begin(); it != jobs_.end() ; it++) {
    foundStatus.insert(myBossIf.status(*it));
    string temp = myBossIf.exitStatus(*it);
    if (temp==string("0")) foundExitCode.insert("SUCCESS");
    else if (temp!=string("")) foundExitCode.insert("FAILURE");
    else foundExitCode.insert("UNKNOWN");
  }
	
  std::vector<string> possStatus;
  std::vector<string> possCodes;
  possCodes.push_back("SUCCESS");
  possCodes.push_back("FAILURE");
  possStatus.push_back("E");
  possStatus.push_back("A");
  possStatus.push_back("R");
  possStatus.push_back("U");
  possStatus.push_back("W");
  possStatus.push_back("K");
  possStatus.push_back("I");
  possStatus.push_back("N");
  possStatus.push_back("T");

  if (header_) cout << "Summary of jobs status" <<endl;
  
  cout.setf(std::ios::left, std::ios::adjustfield);
  if(header_) cout 
    << "Key: " 
    << "E=Ended "
    << "A=Aborted "
    << "R=Running "
    << "U=Unknown "
    << "W=Waiting "
    << "K=Killed "
    << "I=Idle "
    << "N=Not submitted to BOSS "
    << "T=Submitted but database timing out "
    << "?=Unknown"<<endl;

  if(header_) cout 
    << std::setw(9) << "Task ID" 
    << std::setw(4) << "E"
    << std::setw(4) << "A"
    << std::setw(4) << "R"
    << std::setw(4) << "U"
    << std::setw(4) << "W"
    << std::setw(4) << "K"
    << std::setw(4) << "I"
    << std::setw(4) << "N"
    << std::setw(4) << "T"
    << std::setw(4) << "?"<<endl;
  
  cout << std::setw(9) << task_->Id();

  int numPrinted(0);
  for(vector<string>::const_iterator it = possStatus.begin(); it != possStatus.end(); it++) {
    cout << std::setw(4) << foundStatus.count(*it);
    numPrinted = numPrinted+foundStatus.count(*it);
  }
  cout << std::setw(4) <<(foundStatus.size()-numPrinted);
  cout <<endl<<endl;
  
  /*if(header_) cout
    << "Key: " 
    << "SUCCES=Job exit code = 0  "
    << "FAILURE=Job exit code != 0 "
    << "UNKNOWN=Job exit code not known"
    << endl;
  */
  
  if(header_) cout << "Summary of jobs exit status" <<endl;
  if(header_) cout 
    << std::setw(9) << "Task ID"
    << std::setw(9) << "SUCCESS"
    << std::setw(9) << "FAILURE"
    << std::setw(9) << "UNKNOWN"<<endl;
  
  cout << std::setw(9) << task_->Id();

  numPrinted=0;
  for(vector<string>::const_iterator it = possCodes.begin(); it != possCodes.end(); it++) {
    cout << std::setw(9) << foundExitCode.count(*it);
    numPrinted = numPrinted+foundExitCode.count(*it);
  }
  cout << std::setw(4) <<(foundExitCode.size()-numPrinted);
  cout <<endl;
  
  return EXIT_SUCCESS;
}

int QInfoTask::printStatus() const {
  if(!task_||jobs_.empty()) {
    cerr<< " QInfoTask::Ids() Error : no task/jobs defined!"<<endl;
    return EXIT_FAILURE;
  }
  BossIf myBossIf(task_);
  Range jobs(minJob_, maxJob_);
  myBossIf.setJobs(jobs);
  myBossIf.rangeStatus(jobs);
  cout.setf(std::ios::left, std::ios::adjustfield);

  if(header_) cout 
    << std::setw(9)  << "Task ID"
    << std::setw(14) << "GROSS Job ID" 
    << std::setw(13) << "BOSS Status" 
    << std::setw(15) << "Job Exit Code"
    << std::setw(9) << "BOSS ID" 
    << "SCHED ID"<<endl;

  for(vector<Job*>::const_iterator it = jobs_.begin(); it != jobs_.end() ; it++)
    cout 
      << std::setw(9) << task_->Id() 
      << std::setw(14) << (*it)->Id() 
      << std::setw(13) << (myBossIf.status(*it))
      << std::setw(15) << (myBossIf.exitStatus(*it))
      << std::setw(9) << myBossIf.bossId(*it)
      << myBossIf.schedId(*it) <<endl;

  cout <<endl;
  return EXIT_SUCCESS;
}

//Helpful functions for translating user db query into tablename and field
const string dbFieldGetT(const string s) {
  string::size_type idx = s.find('.');
  if(idx == string::npos) return "";
  return s.substr(0, s.find('.'));
}
const string dbFieldGetF(const string s) {
  string::size_type idx = s.find('.');
  if(idx == string::npos) return "";
  return s.substr(s.find('.')+1, s.size());
}
int QInfoTask::printBossDbField(const string fullField) const {
  if(!task_||jobs_.empty()) {
    cerr<< " QInfoTask::printBossDbField() Error : no task/jobs defined!"<<endl;
    return EXIT_FAILURE;
  }

  const string table = dbFieldGetT(fullField);
  const string field = dbFieldGetF(fullField);
  if(Log::level()>2) cout << "QInfoTask::printBossDbField Querying tablename "<<table << " field "<<field<<endl;
  if(field.empty() || table.empty()) {
    cerr<<"QInfoTask::printBossDbField Error - Db Field specification not complete (need tablename.field)"<<endl;
    return EXIT_FAILURE;
  }
  
  BossIf myBossIf(task_);
  
  cout.setf(std::ios::left, std::ios::adjustfield);

  if(header_) cout 
    << std::setw(14) << "GROSS Job ID"
    << fullField << endl;
  for(vector<Job*>::const_iterator it = jobs_.begin(); it != jobs_.end() ; it++) {
    cout << std::setw(14) << (*it)->Id() 
	 << myBossIf.queryBossDb(*it, table, field) <<endl;
  }

  cout <<endl;
  return EXIT_SUCCESS;  
}



