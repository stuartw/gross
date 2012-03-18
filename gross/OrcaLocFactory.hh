#ifndef ORCALOCFACTORY_H
#define ORCALOCFACTORY_H
#include "Log.hh"

#include "TaskFactory.hh"


/*!
  \brief Concrete factory for Orca Local type jobs ("OrcaL").

  This concrete factory will create the appropriate combination of classes (and choose the
  correct wrapper script) for Orca Local type jobs. To decide whether Db or New type classes
  should be instantiated, the Task factory method is overloaded - New types will be created
  if the user specification file is given, Db types will be created if a %Task ID is given.
  This will automatically determine what type of Job to create.

*/

class OrcaLocFactory : public TaskFactory {
public:
  OrcaLocFactory() : newTask_(true) {};
  Task* makeTask(int anId) const;
  Task* makeTask(const File* userCladFile) const;
  //Job* makeJob(const int anId, const int aDataSelect, Task* aTask) const;
  Job* makeJob(const int anId, const vector<int> aDataSelect, Task* aTask) const;
  JDL* makeJDL(const Job* aJob, const CladLookup* aUserSpec, const string aFileName) const;
  Wrapper* makeWrapper(const Job* aJob, CladLookup* aUserSpec, const string aFileName) const;
private:
  mutable bool newTask_; ///< Do we need DbTasks and DbJobs or NewTasks and NewJobs?
};


#endif
