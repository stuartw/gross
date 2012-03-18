#ifndef NEWJOB_H
#define NEWJOB_H
#include "Log.hh"

#include "Job.hh"
class Task;
class Wrapper;
class JDL;

class NewJob : public Job {
public:
  NewJob(const int anId, const int aDataSelect, Task* aTask); //dataSelect is int at the moment as it is just a run num.
  ~NewJob();
  virtual int init();

  virtual int createSubFiles(string aType);
  
  virtual int save();
  virtual int clean();
  
  virtual File* xMLFrag() const {return pXMLFragFile_;};

protected:
  //Setting files for submission
  virtual int createWrapper(string aType);
  virtual int createJDL();
  //Orca Specific
  virtual int createOrcaFiles();
  virtual int saveOrca();
  virtual File* setXMLFrag();
  //Setting the files
  virtual int setInGUIDs();
  virtual int setOutGUIDs();
  virtual int setLocalInFiles();
  virtual int setOutSandboxFiles();
  //Setting other params
  virtual int setExecutable();
  virtual int setUniqSuffix();
  virtual int setStdOutFile();
  virtual int setStdErrFile();
private:
  File* pXMLFragFile_;
};

#endif
