#include "StandardTFactory.hh"

#include "NewTask.hh"
#include "NewJob.hh"
#include "DbTask.hh"
#include "DbJob.hh"

Task* StandardTFactory::makeTask(int myId) const {
  Task* myTask=0;
  if(myId>0) myTask = new DbTask();
  if(myId==0) myTask = new NewTask();
  if(!myTask) cerr<<"StandardTFactory::makeTask Error Undefined Type"<<endl;
  return myTask;
}

Job* StandardTFactory::makeJob(string myType, const int myId, const int myDataSelect, Task* myTask) const {
  Job* myJob;
  if(myType=="DB") myJob = new DbJob(myId, myDataSelect, myTask);
  if(myType=="NEW") myJob = new NewJob(myId, myDataSelect, myTask);
  if(!myTask) cerr<<"StandardTFactory::makeJob Error Undefined Type"<<endl;
  return myJob;
}
