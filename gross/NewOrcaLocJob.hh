#ifndef NEWORCALOCJOB_H
#define NEWORCALOCJOB_H
#include "Log.hh"

#include "OrcaLocJob.hh"
class Task;
class NewOrcaTask;

class NewOrcaLocJob : public OrcaLocJob {
public:
  NewOrcaLocJob(const int anId, const vector<int> aDataSelect, Task* aTask); //dataSelect is int at the moment as it is just a run num.
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
  virtual int setInFiles();
  virtual int setOutFiles();
  virtual int setLocalInFiles(); //other local input files, eg libraries etc...
  //Setting other params
  virtual int setExecutable();
  virtual int setUniqSuffix();
  virtual int setStdOutFile();
  virtual int setStdErrFile();
  virtual int setOutDir();
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
