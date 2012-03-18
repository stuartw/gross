#ifndef DBTASK_H
#define DBTASK_H
#include "Log.hh"

#include "Task.hh"
class File;


class DbTask : public Task {
public:
  virtual int init(const File* userCladFile, const int& anId);
  virtual int prepareJobs(const string& aType);
  virtual int save();
  virtual int queryPrepareJobs();
protected:
  virtual int setJdlRem();
  virtual int createJobs();
  virtual int initJobs();
  virtual int createSubFiles(const string& aType);
  virtual int saveJobs();
};
#endif
