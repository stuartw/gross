#include "Job.hh"

#include "File.hh"
#include "Task.hh"
#include "Wrapper.hh"
#include "JDL.hh"
#include "LocalDb.hh"
#include "DeletePtr.hh"
#include <algorithm>
#include "ContPrint.hh"

Job::Job(const int myId, const int myDataSelect, Task* myTask) 
  : Id_(myId), dataSelect_(myDataSelect), task_(myTask), 
    wrapper_(0), jdl_(0), unInit_(true) {
}
Job::~Job(){
  if(wrapper_) delete wrapper_;
  if(jdl_) delete jdl_;
  //Delete these vectors of pointers (use deletePtr to actually call delete on these objects).
  for_each(vLocalInFiles_.begin(), vLocalInFiles_.end(), deletePtr<File>);
  vLocalInFiles_.erase(vLocalInFiles_.begin(), vLocalInFiles_.end());
  
  for_each(vLocalWrapFiles_.begin(), vLocalWrapFiles_.end(), deletePtr<File>);
  vLocalWrapFiles_.erase(vLocalWrapFiles_.begin(), vLocalWrapFiles_.end());
}

int Job::save() {
  if(!(task_->Id()) || !Id()) {   //ensure Id is non-zero (ie Task and Job have been initialised ok)
    cerr << "Job::save(): Error: Task ID or Job ID set to 0. Save failed!"<<endl;
    return EXIT_FAILURE;
  }
  
  if(clean()) {   //ensure this job entry is unique by deleting previous entries with same job/task ID
    cerr<<"Job::save(): cannot delete previous entry. Save Failed"<<endl;
    return EXIT_FAILURE;
  }
  
  //Job Table
  ostringstream myKeys, myValues;
  myKeys << "TaskID, JobID, DataSelect, ExecName, StdOut, StdErr, Suffix";
  myValues << task_->Id() << "," << Id() << ",'" << dataSelect() << "','" << executable() << "','"
	   << stdOutFile() << "','" << stdErrFile() << "','" << uniqSuffix() <<"'";
  if(LocalDb::instance()->tableSave("Analy_Job", myKeys.str(), myValues.str())) return EXIT_FAILURE; 
  
  //LocalIn Table
  for (set<File*>::const_iterator i=(localInFiles()).begin(); i!= (localInFiles()).end(); i++){
    myValues.str("");
    myValues << task_->Id() << "," << Id() << ",'"<< (*i)->fullHandle()<<"'";
    if(LocalDb::instance()->tableSave("Analy_LocalIn", "TaskId, JobID, Name", myValues.str())) return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int Job::clean() { //deletes all entries in all tables for this JOB and TASK ID
  ostringstream sel;
  sel << "TaskID="<<task_->Id()<<"&&JobID="<<Id();

  if( LocalDb::instance()->tableDelete("Analy_Job", sel.str()) || 
      LocalDb::instance()->tableDelete("Analy_LocalIn", sel.str()) ) {
    cerr << "Job::delete: Error deleting previous entry!"<<endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void Job::print() const {
  cout << "Id = "<<Id() <<endl;
  cout << "Data Selection = "<<dataSelect() <<endl;
  cout << "Unique Suffix = " <<uniqSuffix()<<endl;
  cout << "Std Output File = "<<stdOutFile() <<endl;
  cout << "Std Error File = "<<stdErrFile() <<endl;
  cout << "Local Input Files = " << localInFiles() <<endl;
}


