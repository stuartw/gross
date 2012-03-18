#ifndef DBORCAGJOB_H
#define DBORCAGJOB_H
#include "Log.hh"

#include "OrcaGJob.hh"
class Task;

class DbOrcaGJob : public OrcaGJob {
public:
  DbOrcaGJob(const int anId, const vector<int> aDataSelect, Task* aTask); //dataSelect is int at the moment as it is just a run num.
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
  virtual int setInGUIDs();
  virtual int setOutGUIDs();
  virtual int setLocalInFiles();
  virtual int setOutSandboxFiles();
  //Setting other params
  virtual int setExecutable();
  virtual int setUniqSuffix();
  virtual int setStdOutFile();
  virtual int setStdErrFile();
};

#endif
