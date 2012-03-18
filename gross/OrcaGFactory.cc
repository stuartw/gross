#include "OrcaGFactory.hh"

#include "NewOrcaTask.hh"
#include "DbOrcaTask.hh"
#include "NewOrcaGJob.hh"
#include "DbOrcaGJob.hh"
#include "OrcaJDL.hh"
#include "OrcaGWrapper.hh"

Task* OrcaGFactory::makeTask(int myId) const {
  newTask_ = false;
  return new DbOrcaTask(myId);
}
Task* OrcaGFactory::makeTask(const File* userCladFile) const {
  newTask_ = true;
  return new NewOrcaTask(userCladFile);
}
Job* OrcaGFactory::makeJob(const int myId, const int myDataSelect, Task* myTask) const {
  if(newTask_) return new NewOrcaGJob(myId, myDataSelect, myTask); 
  else return new DbOrcaGJob(myId, myDataSelect, myTask);
}
JDL* OrcaGFactory::makeJDL(const Job* aJob, const CladLookup* aUserSpec, const string aFileName) const {
  const OrcaGJob* pOGJ=dynamic_cast<const OrcaGJob*> (aJob); //This is safe as Factory controls what derived object aJob points to
  if(!aJob) cerr << "OrcaGFactory::makeJDL() Error: cannot initialise OrcaG Job\n";  
  return new OrcaJDL(pOGJ, aUserSpec, aFileName);
}
Wrapper* OrcaGFactory::makeWrapper(const Job* aJob, CladLookup* aUserSpec, const string aFileName) const {
  const OrcaGJob* pOGJ=dynamic_cast<const OrcaGJob*> (aJob); //This is safe as Factory controls what derived object aJob points to
  if(!aJob) cerr << "OrcaGFactory::makeWrapper() Error: cannot initialise OrcaG Job\n";  
  //Note that facType() is effectively hard coded in here. This forces requirement of script name being identical to factory type name.
  //A reduction in flexibility, but a simpler design.
  return new OrcaGWrapper(pOGJ, aUserSpec, aFileName, facType()); 
}
