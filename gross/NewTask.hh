#ifndef NEWTASK_H
#define NEWTASK_H
#include "Log.hh"

#include "Task.hh"
class File;

class NewTask : public Task {
public:
  NewTask(const File* userCladFile); ///<New Task always constructed from File
  virtual ~NewTask()=0; ///<pABC 
  int split() =0; ///<No standard split method for new tasks - defined in derived classes
  int makeSubFiles(); 
  int save();
protected:
  virtual int saveJobs();
};

#endif
