#ifndef DBORCALOCJOB_H
#define DBORCALOCJOB_H
#include "Log.hh"

#include "OrcaLocJob.hh"
class Task;

class DbOrcaLocJob : public OrcaLocJob {
public:
  DbOrcaLocJob(const int anId, const vector<int> aDataSelect, Task* aTask); //dataSelect is int at the moment as it is just a run num.
  virtual int makeSubFiles();
  int save();

protected:
  //Setting files for submission
  virtual int createWrapper();
  virtual int createJDL();
  //Orca Specific
  virtual File* setXMLFrag();
  virtual int createOrcaFiles();
  //Setting the files
  virtual int setInFiles();
  virtual int setOutFiles();
  virtual int setLocalInFiles();
  //Setting other params
  virtual int setExecutable();
  virtual int setUniqSuffix();
  virtual int setStdOutFile();
  virtual int setStdErrFile();
  virtual int setOutDir();
};

#endif
