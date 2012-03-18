#include "Task.hh"
#include "CladLookup.hh"
#include "PhysCat.hh"
#include "Job.hh"
#include "BossIf.hh"
#include "File.hh"
#include "LocalDb.hh"
#include "DeletePtr.hh"
#include <algorithm>

Task::Task() : userSpec_(0), physCat_(0), Id_(0){
}
Task::~Task(){
  if(userSpec_) delete userSpec_;
  if(physCat_) delete physCat_;
  jobs_.erase(remove_if(jobs_.begin(), jobs_.end(), deletePtr<Job>), jobs_.end());  
  //DOES NOT WORK - CAN'T TAKE DTOR ADDRESS for_each(jobs_.begin(), jobs_.end(), std::mem_fun(&Job::~Job));
}
int Task::createSubFiles(const string& myType){
  if(jobs_.empty()) {
    cerr<<"Task::createSubFiles() Error: no jobs defined"<<endl;
    return EXIT_FAILURE;
  }

  for(vector<Job*>::const_iterator i = jobs_.begin(); i!=jobs_.end(); i++) {
    if(Log::level()>2) cout <<"Task::createSubFiles() Creating submission files for job with id " << (*i)->Id() <<endl;
    if((*i)->createSubFiles(myType)) return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
int Task::saveJobs() {
  if(jobs_.empty()) {
    cerr<<"Task::saveJobs() Error: no jobs defined"<<endl;
    return EXIT_FAILURE;
  }
  for(vector<Job*>::const_iterator i = jobs_.begin(); i!=jobs_.end(); i++) {
    if(Log::level()>0) cout <<"Task::saveJobs() Saving Job " << (*i)->Id() <<endl;
    if((*i)->save()) return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;  
}
int Task::save(){
  if(Id_) {  //Already saved in db
    if(Log::level()>2) cout <<"Task::save() Task already saved in db"<<endl;
    return Id_;
  }
  if(userClad_=="" || ! userSpec_) {  //Task::init has not succeeded
    cerr<<"Task::save() Error Cannot save undefined task"<<endl;
    return Id_;
  }

  //Get escaped string of contents to be saved
  string escClad = LocalDb::instance()->escapeString(userClad_);
  string escJDLDump = LocalDb::instance()->escapeString(userSpec_->jdlDump());

  if(escClad.empty()||escJDLDump.empty()) return EXIT_FAILURE;

  string query = "INSERT INTO Analy_Task (UserSpec, JDLRem) VALUES (" 
    + escClad + "," + escJDLDump + ")";

  if(mysql_query(LocalDb::instance()->connection(), query.c_str())) {
    cerr << "Task::save(): Error saving Task user spec to Db" 
	 << mysql_error(LocalDb::instance()->connection()) << endl;
    return Id_;
  }
  Id_ = mysql_insert_id(LocalDb::instance()->connection());
  if(Log::level()>0) cout << "Task::save(): Saving Task user spec into DB with ID "<< Id_<<endl;

  if(this->saveJobs()) return 0; //Done here to ensure jobs and task both saved together for consistency
  return Id_;
}
const Job* Task::job(int myJobId) {
  Job* pJob = 0;
  if(jobs_.empty()) {
    cerr<<"Task::job() Error: no jobs defined"<<endl;
    return pJob;
  }

  for(vector<Job*>::const_iterator i = jobs_.begin(); i!=jobs_.end(); i++)
    if((*i)->Id()==myJobId) pJob = *i;
  return pJob;  
}
int Task::queryPrepareJobs() {
  cout <<"Task::queryPrepareJobs() ERROR: No status info - can only query jobs saved to Db"<<endl;
  return EXIT_FAILURE;
}

