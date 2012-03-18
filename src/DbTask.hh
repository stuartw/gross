#ifndef DBTASK_H
#define DBTASK_H
#include "Log.hh"

#include "Task.hh"

class DbTask : public Task {
public:
  DbTask(const int& anId); ///<If Task is on DB, then it must have an ID
  virtual ~DbTask()=0; ///<pABC
  int split() =0; ///<Define in derived types, not here.
  int makeSubFiles();
  int delSubFiles();
  int save();
protected:
  virtual int setJdlRem();
  virtual int saveJobs();
};
#endif
