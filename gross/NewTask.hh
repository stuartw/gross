#ifndef NEWTASK_H
#define NEWTASK_H
#include "Log.hh"

#include "Task.hh"
class File;

class NewTask : public Task {
public:
  virtual int init(const File* userCladFile, const int& anId);
  virtual int prepareJobs(const string& aType);
  virtual int save();
protected:
  virtual int createJobs();
  virtual int initJobs();
  virtual int createSubFiles(const string& aType);
  virtual int saveJobs();
};

#endif
