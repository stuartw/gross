#include "DbTask.hh"

#include "Job.hh"
#include "CladLookup.hh"
#include "LocalDb.hh"

DbTask::~DbTask() {};

//Explicitly inherit from Task base class (safest way to do this!)
int DbTask::save() {return Task::save();}
int DbTask::saveJobs() {return Task::saveJobs();}


DbTask::DbTask(const int& myId) : Task() {
  unInit_=true;
  if(!myId) return; //non-zero ID required
  if(Log::level()>2) cout<<"DbTask::DbTask(): creating new userSpec from database with id "<< myId <<endl;
  
  ostringstream os;
  os << "ID="<<myId;
  vector<string> myResults;
  LocalDb::instance()->tableRead("Analy_Task", "UserSpec", os.str(), myResults);
  if(myResults.size()!=1) {
    cerr<<"DbTask::DbTask() Error reading UserSpec from Db"<<endl;
    return;
  }
  if(!myResults[0].empty()) userClad(myResults[0]);
  else {
    cerr<<"DbTask::ctor Error - UserSpec not defined in DB"<<endl;
    return;
  }

  userSpec(new CladLookup(userClad())); //deleted in ~Task
  Id(myId); //setting ID value shows that Task does not require saving again

  unInit_=false;
}

int DbTask::makeSubFiles() {
  if(setJdlRem()) return EXIT_FAILURE; //Set Task's userspec with the JDL Remnant. Only required if JDL to be written out.
  
  return Task::makeSubFiles();
}
int DbTask::delSubFiles() {
  return Task::delSubFiles();
}
int DbTask::setJdlRem() {
  //This needs to be taken from Db and set within userSpec as the JDL object uses 
  //the CladLookup jdlDump method to create the JDL file for a job (it does not read
  //it from the Db itself).
  if(!Id() || !userSpec()) {
    cerr<<"DbTask::setJdlRem() Error Task not initialised properly"<<endl;
    return EXIT_FAILURE;
  }
  ostringstream os;
  os << "ID="<<Id();
  
  vector<string> myResults;
  LocalDb::instance()->tableRead("Analy_Task", "JDLRem", os.str(), myResults);
  if(myResults.size()!=1) {
    cerr<<"DbTask::setJdlRem() Error reading JdlRem from Db"<<endl;
    return EXIT_FAILURE;
  }
  userSpec()->jdlDump(myResults[0]);
  
  return EXIT_SUCCESS;
}
