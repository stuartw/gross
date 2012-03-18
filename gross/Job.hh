#ifndef JOB_H
#define JOB_H
#include "Log.hh"

class File;
class Wrapper;
class JDL;
class Task;

/*!
  \brief pABC that represents a job

  The Job object contains all information for running a job. Its operations are simply
  to create its required submission files (through creating Wrapper and JDL classes) and to make
  itself persistent.

  Job itself is a pABC defining the interface for a generic Job object.

  Note on design: as you go down the inheritance tree, sub-classes obtain additional attributes and 
  behaviour. These are defined at the first level in which they are appropriate. So a list of Sandbox
  files are appropriate at the OrcaGJob level and a PhysCat object is appropriate specifically for the
  NewOrcaGJob concrete class. As a lot of the functionality of job is to provide appropriate getter 
  functions to other client classes (particularly JDL and Wrapper classes), the extended attributes
  lead to an extended interface as you go down the inheritance tree. For this reason, the OrcaGJob 
  class layer is used - to maintain as common an interface as possible as low down the tree as possible.

  Note on containers: the set container is used extensively to hold lists of files or filenames 
  as it has the useful feature of not allowing duplicate entries in it.

*/

class Job {
public:
  Job(const int anId, const vector<int> aDataSelect, Task* aTask); ///<dataSelect is int at the moment as it is just a run num.
  virtual ~Job() = 0; ///<pABC
  virtual int makeSubFiles() = 0; ///<create local submission files
  virtual int save() = 0;
  virtual int clean() = 0;

  /*!
    \brief Check initialisation of object has been completed successfully.
    
    Client classes must check that ctor succeeded using this method. This is required
    as the derived class ctors may well fail - eg they might contact remote DBs. 
    For this to work, derived class ctors must set unInit_ to false after their ctor
    has successfully initialised the derived object. Note that derived classes of derived classes 
    must set the flag to false themselves at the beginning of their ctor and then to true once their ctor
    has successfully completed.
    Exceptions could also be used here, but this is easiest for the moment.
  */
  bool operator!() {if(unInit_) cerr <<"Job::operator!() Error: job initialisation failure!\n"; return unInit_;}

  //Getters
  const vector <int>& dataSelect() const {return dataSelect_;}
  const int& Id() const {return Id_;}
  const string& executable() const {return executable_;}
  const string& uniqSuffix() const {return uniqSuffix_;}
  const string& stdOutFile() const {return stdOutFile_;}
  const string& stdErrFile() const {return stdErrFile_;}
  const string wrapperName() const;
  const string wrapperFullPathName() const;
  const set<File*>& localInFiles() const {return vLocalInFiles_;}
  const set<File*>& localWrapFiles() const {return vLocalWrapFiles_;}
  const JDL* jdl() const {return jdl_;}  
  
  virtual void print() const;

protected:
  //Data members (and they're not private... ug!)
  const int Id_;
  const vector<int> dataSelect_;
  Task* task_;
  Wrapper* wrapper_;
  JDL* jdl_;
  string executable_;
  string uniqSuffix_; ///<Unique suffix to add to end of output file names
  string stdOutFile_; ///<Where to send standard out from job
  string stdErrFile_; ///<Where to send standard err from job
  //File containers
  set<File*> vLocalInFiles_; ///<Local input file
  set<File*> vLocalWrapFiles_; ///<Local wrapper files (ie files produced by GROSS)
  //Initialisation tag
  bool unInit_;
private:
  //No implementation for
  Job(const Job&);
  Job& operator=(const Job&);
};

#endif
