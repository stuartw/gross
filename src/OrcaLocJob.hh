#ifndef ORCALOCJOB_H
#define ORCALOCJOB_H
#include "Log.hh"

#include "Job.hh"
class Task;
class File;
class Site;

class OrcaLocJob : public Job {
public:
  OrcaLocJob(const int anId, const vector<int> aDataSelect, Task* aTask); //dataSelect is int at the moment as it is just a run num.
  virtual ~OrcaLocJob() = 0; //pABC
  virtual int makeSubFiles() =0;  //Defined in sub-classes
  virtual int delSubFiles() =0; 
  virtual int save();
  virtual int clean();

  //Getters
  const set<string>& outFiles() const {return vOutFiles_;};
  //const set<string>& inFiles() const {return vInFiles_;};
  virtual File* tarFile() const {return pTarFile_;};
  const string orcaVers() const {return orcaVers_;};
  const string& outDir() const {return outDir_;} 
  const vector<Site*> sites() const {return vSites;};

  void print() const;

protected:
  //File* pXMLFragFile_;
  string outDir_;
  File* pTarFile_;
  string orcaVers_;
    
  //set<string> vInFiles_;
  vector<Site*> vSites;
  set<string> vOutFiles_;
};

#endif
