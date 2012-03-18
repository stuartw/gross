#include "Job.hh"

#include "File.hh"
#include "Task.hh"
#include "Wrapper.hh"
#include "JDL.hh"
#include "LocalDb.hh"
#include "DeletePtr.hh"
#include <algorithm>

Job::Job() {}; //Required to be defined for pABC

Job::Job(const int myId, const int myDataSelect, Task* myTask) 
  : Id_(myId), dataSelect_(myDataSelect), task_(myTask), 
    wrapper_(0), jdl_(0) {
}
Job::~Job(){
  if(wrapper_) delete wrapper_;
  if(jdl_) delete jdl_;
  //Delete all these vectors of pointers (use deletePtr to actually call delete on these objects).
  vLocalInFiles_.erase(remove_if(vLocalInFiles_.begin(), vLocalInFiles_.end(), deletePtr<File>), vLocalInFiles_.end());
  vLocalWrapFiles_.erase(remove_if(vLocalWrapFiles_.begin(), vLocalWrapFiles_.end(), deletePtr<File>), vLocalWrapFiles_.end());
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
  myKeys << "TaskID, JobID, DataSelect, ExecName, StdOut, StdErr, Suffix, MetaFile";
  myValues << task_->Id() << "," << Id() << ",'" << dataSelect() << "','" << executable() << "','"
	   << stdOutFile() << "','" << stdErrFile() << "','" << uniqSuffix() << "','" << metaFile()<<"'";
  if(LocalDb::instance()->tableSave("Analy_Job", myKeys.str(), myValues.str())) return EXIT_FAILURE; 
  //InGUIDs Table
  myKeys.str("");
  myKeys << "TaskID, JobID, Name";
  for (vector<string>::const_iterator i=(inGUIDs()).begin(); i!= (inGUIDs()).end(); i++){
    myValues.str("");
    myValues << task_->Id() << "," << Id() << ",'"<< (*i)<<"'";
    if(LocalDb::instance()->tableSave("Analy_InGUIDs", myKeys.str(), myValues.str())) return EXIT_FAILURE;
  }
  //OutGUIDs Table
  for (vector<string>::const_iterator i=(outGUIDs()).begin(); i!= (outGUIDs()).end(); i++){
    myValues.str("");
    myValues << task_->Id() << "," << Id() << ",'"<< (*i)<<"'";
    if(LocalDb::instance()->tableSave("Analy_OutGUIDs", myKeys.str(), myValues.str())) return EXIT_FAILURE;
  }
  //LocalIn Table
  for (vector<File*>::const_iterator i=(localInFiles()).begin(); i!= (localInFiles()).end(); i++){
    myValues.str("");
    myValues << task_->Id() << "," << Id() << ",'"<< (*i)->fullHandle()<<"'";
    if(LocalDb::instance()->tableSave("Analy_LocalIn", myKeys.str(), myValues.str())) return EXIT_FAILURE;
  }
  //OutSandbox Table
  for (vector<string>::const_iterator i=(outSandboxFiles()).begin(); i!= (outSandboxFiles()).end(); i++){
    myValues.str("");
    myValues << task_->Id() << "," << Id() << ",'"<< (*i)<<"'";
    if(LocalDb::instance()->tableSave("Analy_OutSandbox", myKeys.str(), myValues.str())) return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int Job::clean() { //deletes all entries in all tables for this JOB and TASK ID
  ostringstream sel;
  sel << "TaskID="<<task_->Id()<<"&&JobID="<<this->Id();
  if( LocalDb::instance()->tableDelete("Analy_Job", sel.str()) || 
      LocalDb::instance()->tableDelete("Analy_InGUIDs", sel.str()) || 
      LocalDb::instance()->tableDelete("Analy_LocalIn", sel.str()) || 
      LocalDb::instance()->tableDelete("Analy_OutGUIDs", sel.str()) || 
      LocalDb::instance()->tableDelete("Analy_OutSandbox", sel.str()) ) {
    cerr << "Job::delete: Error deleting previous entry!"<<endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

//Global functions to help output Jobs
ostream& operator<<(ostream& os, const vector<string>& myVec) {
  for(vector<string>::const_iterator it=myVec.begin(); it!=myVec.end();it++)
    os<<(*it)<<" ";
  return os;
}
ostream& operator<<(ostream& os, const vector<File*>& myVec) {
  for(vector<File*>::const_iterator it=myVec.begin(); it!=myVec.end();it++)
    os<<(*it)->fullHandle()<<" ";
  return os;
}
ostream& operator<<(ostream& os, Job& myJ) {
  os << "Id = "<<myJ.Id() <<endl;
  os << "Data Selection = "<<myJ.dataSelect() <<endl;
  os << "Unique Suffix = " <<myJ.uniqSuffix()<<endl;
  os << "Std Output File = "<<myJ.stdOutFile() <<endl;
  os << "Std Error File = "<<myJ.stdErrFile() <<endl;
  os << "Local Input Files = " << myJ.localInFiles() <<endl;
  os << "Input Data File LFNs = " << myJ.inGUIDs() <<endl;
  os << "Output Sandbox Files = " << myJ.outSandboxFiles() <<endl;
  os << "Output Data File LFNs = " << myJ.outGUIDs() <<endl;
  return os;
}


