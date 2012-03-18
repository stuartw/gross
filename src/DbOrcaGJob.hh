#ifndef DBORCAGJOB_H
#define DBORCAGJOB_H
#include "Log.hh"
class Sites;

#include "OrcaGJob.hh"
class Task;
class DbOrcaTask;

class DbOrcaGJob : public OrcaGJob {
public:
  DbOrcaGJob(const int anId, const vector<int> aDataSelect, Task* aTask); //dataSelect is int at the moment as it is just a run num.
  virtual int makeSubFiles();
  virtual int delSubFiles();
  int save();

protected:
  //Setting files for submission
  virtual int createWrapper();
  virtual int createJDL();
  //Orca Specific
  virtual int createOrcaFiles();
  virtual File* setTarFile();
  virtual int setOrcaVers();
  //Setting the files
  //virtual int setInGUIDs();
  virtual int setOutGUIDs();
  virtual int setLocalInFiles();
  virtual int setOutSandboxFiles();
  //Setting other params
  virtual int setExecutable();
  virtual int setUniqSuffix();
  virtual int setStdOutFile();
  virtual int setStdErrFile();
  virtual int setSites();
private:
};

#endif
