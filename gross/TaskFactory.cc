#include "TaskFactory.hh"

#include "StandardTFactory.hh"

TaskFactory* TaskFactory::instance_=0;

TaskFactory* TaskFactory::instance() {
  if(instance_==0) {
    instance_ = new StandardTFactory(); //Can subclass this when required
  }
  return instance_;
}

//This interface designed to be over-ridden
Task* TaskFactory::makeTask(int anId) const {
  Task* myNullTask = 0;
  cerr << "TaskFactory::makeTask Error this should never be called!"<<endl;
  return myNullTask;
}
Job* TaskFactory::makeJob(string aType, const int anId, const int aDataSelect, Task* aTask) const {
  Job* myNullJob = 0;
  cerr << "TaskFactory::makeJob Error this should never be called!"<<endl;
  return myNullJob;
}

