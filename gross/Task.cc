#include "Task.hh"
#include "Job.hh"
#include "LocalDb.hh"
#include "DeletePtr.hh"
#include <algorithm>
#include "CladLookup.hh"
#include "TaskFactory.hh"

Task::Task() : userSpec_(0), Id_(0), userClad_(""), unInit_(true) {}

Task::~Task(){
  if(userSpec_) delete userSpec_;
  //delete container of pointers to Jobs:
  jobs_.erase(remove_if(jobs_.begin(), jobs_.end(), deletePtr<Job>), jobs_.end());
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

  string query = "INSERT INTO Analy_Task (UserSpec, JDLRem, FacType) VALUES (" 
    + escClad + "," + escJDLDump + "," + "'" + TaskFactory::facType() + "'" + ")";

  if(mysql_query(LocalDb::instance()->connection(), query.c_str())) {
    cerr << "Task::save(): Error saving Task user spec to Db" 
	 << mysql_error(LocalDb::instance()->connection()) << endl;
    return Id_;
  }
  Id_ = mysql_insert_id(LocalDb::instance()->connection());
  if(Log::level()>0) cout << "Task::save(): Saving Task user spec into DB with ID "<< Id_<<endl;

  if(saveJobs()) return 0; //Done here to ensure jobs and task both saved together for consistency
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

int Task::makeSubFiles(){
  if(jobs_.empty()) {
    cerr<<"Task::makeSubFiles() Error: no jobs defined"<<endl;
    return EXIT_FAILURE;
  }

  for(vector<Job*>::const_iterator i = jobs_.begin(); i!=jobs_.end(); i++) {
    if(Log::level()>2) cout <<"Task::makeSubFiles() Creating submission files for job with id " << (*i)->Id() <<endl;
    if((*i)->makeSubFiles()) return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
