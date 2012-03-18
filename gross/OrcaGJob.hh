#ifndef ORCAGJOB_H
#define ORCAGJOB_H
#include "Log.hh"

#include "Job.hh"
#include "OrcaGJob.hh"
class Task;
class File;

class OrcaGJob : public Job {
public:
  OrcaGJob(const int anId, const int aDataSelect, Task* aTask); //dataSelect is int at the moment as it is just a run num.
  virtual ~OrcaGJob() = 0; //pABC
  virtual int makeSubFiles() =0;  //Defined in sub-classes
  virtual int save();
  virtual int clean();
    
  //Getters
  const set<string>& outGUIDs() const {return vOutGUIDs_;};
  const set<string>& inGUIDs() const {return vInGUIDs_;};  
  const set<string>& outSandboxFiles() const {return vOutSandboxFiles_;};
  const string& metaFile() const {return metaFile_;};
  virtual File* xMLFrag() const {return pXMLFragFile_;};
  
  void print() const;

protected:
  File* pXMLFragFile_;
  
  string metaFile_;
  set<string> vOutSandboxFiles_; 
  set<string> vInGUIDs_;
  set<string> vOutGUIDs_;
};

#endif
