#ifndef RETRIEVE_H
#define RETRIEVE_H
#include "Log.hh"

class Retrieve; //forward declare (Wrapper needs Job and Job needs Wrapper)
class File;

/*!
  \brief Class to write out the auto retrieve script

*/

class Retrieve {
public:
  Retrieve(const string myExecName, const string aType); ///<Type here means type of script stored on database (set by -scriptName when registering script)
  virtual ~Retrieve(); ///<pABC
  virtual const string execName() const {return execName_;};
  virtual const string execNameFullHandle() const {return execNameFullHandle_;};
  virtual const set<File*>& files() const {return filesCreated_;};
  virtual const int save(string aDir); ///<Creates the files
protected:
  virtual int script();
  set<File*> filesCreated_; ///<Keeps track of any physical files created by object
  const string type_;
  string script_;
private:
  const string execName_;
  string execNameFullHandle_;  
  //No implementation
  Retrieve(const Retrieve&);
  Retrieve& operator=(const Retrieve&);
};


#endif
