#include "OrcaLocFactory.hh"

#include "NewOrcaTask.hh"
#include "DbOrcaTask.hh"
#include "NewOrcaLocJob.hh"
#include "DbOrcaLocJob.hh"
#include "OrcaLocJDL.hh"
#include "OrcaLocWrapper.hh"

Task* OrcaLocFactory::makeTask(int myId) const {
  newTask_ = false;
  return new DbOrcaTask(myId);
}
Task* OrcaLocFactory::makeTask(const File* userCladFile) const {
  newTask_ = true;
  return new NewOrcaTask(userCladFile);
}
/*Job* OrcaLocFactory::makeJob(const int myId, const int myDataSelect, Task* myTask) const {
  if(newTask_) return new NewOrcaLocJob(myId, myDataSelect, myTask);
  else return new DbOrcaLocJob(myId, myDataSelect, myTask);
}*/
Job* OrcaLocFactory::makeJob(const int myId, const vector<int> myDataSelect, Task* myTask) const {
	  if(newTask_) return new NewOrcaLocJob(myId, myDataSelect, myTask);
	    else return new DbOrcaLocJob(myId, myDataSelect, myTask);
}
JDL* OrcaLocFactory::makeJDL(const Job* aJob, const CladLookup* aUserSpec, const string aFileName) const {
  const OrcaLocJob* pOLOCJ=dynamic_cast<const OrcaLocJob*> (aJob); //This is safe as Factory controls what derived object aJob points to
  if(!aJob) cerr << "OrcaLocFactory::makeJDL() Error: cannot initialise OrcaLoc Job\n";
  return new OrcaLocJDL(pOLOCJ, aUserSpec, aFileName);
}
Wrapper* OrcaLocFactory::makeWrapper(const Job* aJob, CladLookup* aUserSpec, const string aFileName) const {
  const OrcaLocJob* pOLOCJ=dynamic_cast<const OrcaLocJob*> (aJob); //This is safe as Factory controls what derived object aJob points to
  if(!aJob) cerr << "OrcaLocFactory::makeWrapper() Error: cannot initialise OrcaG Job\n";
  //Note that facType() is effectively hard coded in here. This forces requirement of script name being identical to factory type name.
  //A reduction in flexibility, but a simpler design.
  return new OrcaLocWrapper(pOLOCJ, aUserSpec, aFileName, facType());
}
