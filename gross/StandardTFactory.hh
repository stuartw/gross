#ifndef STANDARDTFACTORY_H
#define STANDARDTFACTORY_H
#include "Log.hh"

#include "TaskFactory.hh"

class StandardTFactory : public TaskFactory {
public:
  StandardTFactory() {};
  virtual Task* makeTask(int anId) const;
  virtual Job* makeJob(string aType, const int anId, const int aDataSelect, Task* aTask) const;
};


#endif
