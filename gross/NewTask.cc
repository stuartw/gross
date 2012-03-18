#include "NewTask.hh"

#include "CladLookup.hh"
#include "Job.hh"
#include "File.hh"

//Explicitly inherit from base class (safest way to do this according to Scott Meyer!)
int NewTask::save() { return Task::save();}
int NewTask::saveJobs() {return Task::saveJobs();}
int NewTask::makeSubFiles() {return Task::makeSubFiles();}

NewTask::~NewTask() {};

NewTask::NewTask(const File* myUserSpecFile) : Task() {
  unInit_=true;
  if(!myUserSpecFile->exists()) {
    cerr <<"NewTask::NewTask(): Error: user spec file does not exist!"<<endl;
    return;
  }
  if(Log::level()>2) cout<<"NewTask::NewTask(): creating new userSpec from user specified file "
			 << myUserSpecFile->fullHandle()<< endl;
  userClad(myUserSpecFile->contents());
  userSpec(new CladLookup(userClad())); //deleted in ~Task
  unInit_=false;
}

