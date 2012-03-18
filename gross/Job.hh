#ifndef JOB_H
#define JOB_H
#include "Log.hh"

class File;
class Wrapper;
class JDL;
class Task;

class Job {
public:
  Job();
  Job(const int anId, const int aDataSelect, Task* aTask); //dataSelect is int at the moment as it is just a run num.
  virtual ~Job() = 0; //pABC
  
  virtual int init() = 0;
  
  virtual int createSubFiles(string aType) = 0;

  virtual File* xMLFrag() const = 0;

  virtual int save() = 0;
  virtual int clean() = 0;

  //Getters
  const int& dataSelect() const {return dataSelect_;};
  const int& Id() const {return Id_;};
  const string& executable() const {return executable_;};
  const string& uniqSuffix() const {return uniqSuffix_;};
  const string& stdOutFile() const {return stdOutFile_;};
  const string& stdErrFile() const {return stdErrFile_;};
  const string& metaFile() const {return metaFile_;};
  const vector<string>& outGUIDs() const {return vOutGUIDs_;};
  const vector<File*>& localInFiles() const {return vLocalInFiles_;};
  const vector<File*>& localWrapFiles() const {return vLocalWrapFiles_;};
  const vector<string>& outSandboxFiles() const {return vOutSandboxFiles_;};
  const vector<string>& inGUIDs() const {return vInGUIDs_;};  
  const JDL* jdl() const {return jdl_;};
  
  friend std::ostream& operator<<(std::ostream&, Job&);

protected:
  //Data members (and they're not private... ug!)
  const int Id_;
  const int dataSelect_;
  Task* task_;
  Wrapper* wrapper_;
  JDL* jdl_;
  string executable_;
  string uniqSuffix_;
  string stdOutFile_;
  string stdErrFile_;
  string metaFile_;
  //File containers
  vector<string> vOutGUIDs_;
  vector<File*> vLocalInFiles_;
  vector<File*> vLocalWrapFiles_;
  vector<string> vOutSandboxFiles_; 
  vector<string> vInGUIDs_;
private:
  //No implementation for
  Job(const Job&);
  Job& operator=(const Job&);
};

#endif
