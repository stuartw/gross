#ifndef TASKFACTORY_H
#define TASKFACTORY_H

#include "Log.hh"

class Task;
class Job;

class TaskFactory {
public:
  static TaskFactory* instance();
  virtual Task* makeTask(int anId) const;
  virtual Job* makeJob(string aType, const int anId, const int aDataSelect, Task* aTask) const;
protected:
  TaskFactory() {}; //Singleton
private:
  static TaskFactory* instance_;  
};


#endif
