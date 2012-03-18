#ifndef ORCAGJOB_H
#define ORCAGJOB_H
#include "Log.hh"

#include "Job.hh"
class Task;
class File;
class Site;

class OrcaGJob : public Job {
public:
  OrcaGJob(const int anId, const vector<int> aDataSelect, Task* aTask); //dataSelect is a vector of int at the moment as it is just a listof  run num.
  virtual ~OrcaGJob() = 0; //pABC
  virtual int makeSubFiles() =0;  //Defined in sub-classes
  virtual int delSubFiles() = 0; //defined in sub-classes
  virtual int save();
  virtual int clean();
    
  //Getters
  const set<string>& outGUIDs() const {return vOutGUIDs_;};
  //const set<string>& inGUIDs() const {return vInGUIDs_;};  
  const set<string>& outSandboxFiles() const {return vOutSandboxFiles_;};
  //const set<string> metaFile() const {return vmetaFile_;};
  //virtual File* xMLFrag() const {return pXMLFragFile_;};  
  virtual File* tarFile() const {return pTarFile_;};
  const string orcaVers() const {return orcaVers_;};
  const vector<Site*> sites() const {return vSites;};
  
  void print() const;

protected:
  //File* pXMLFragFile_;
  File* pTarFile_;  
  string orcaVers_;
  
  //set<string> vmetaFile_;
  set<string> vOutSandboxFiles_; 
  //set<string> vInGUIDs_;
  set<string> vOutGUIDs_;
  vector<Site*> vSites;
  
};

#endif
