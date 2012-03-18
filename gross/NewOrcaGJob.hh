#ifndef NEWORCAGJOB_H
#define NEWORCAGJOB_H
#include "Log.hh"

#include "OrcaGJob.hh"
class Task;
class NewOrcaTask;

class NewOrcaGJob : public OrcaGJob {
public:
  NewOrcaGJob(const int anId, const vector<int> aDataSelect, Task* aTask); //dataSelect is int at the moment as it is just a run num.
  virtual int makeSubFiles();  
  int save();
  
protected:
  //Setting files for submission
  virtual int createWrapper();
  virtual int createJDL();
  //Orca Specific
  virtual int createOrcaFiles();
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
  /*!
    \brief NewOrcaTask type pointer
    
    While get a pointer to base class Task, the NewOrcaTask has an extended interface
    that this class needs to access. So need to make a dynamic cast of pointer to 
    NewOrcaTask type.

  */
  NewOrcaTask* myNewOrcaTask_;
};

#endif
