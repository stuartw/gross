#ifndef WRAPPER_H
#define WRAPPER_H
#include "Log.hh"

class CladLookup;
class Wrapper; //forward declare (Wrapper needs Job and Job needs Wrapper)
class Job; //forward declare (Wrapper needs Job and Job needs Wrapper)
class File;

class Wrapper {
public:
  Wrapper(const Job* aJob, CladLookup* aUserSpec, const string myExecName, const string aType);
  ~Wrapper();
  const string execName() const {return execName_;};
  const string execNameFullHandle() const {return execNameFullHandle_;};
  const vector<File*>& files() const {return filesCreated_;};
  const int save(string aDir);
private:
  int script();
  int steer();
  const Job* job_;
  CladLookup* userSpec_;
  vector<File*> filesCreated_; //Keeps track of any physical files created by object
  const string execName_;
  string execNameFullHandle_;  
  const string type_;
  string script_;
  string steer_;
  //No implementation
  Wrapper(const Wrapper&);
  Wrapper& operator=(const Wrapper&);
};


#endif
