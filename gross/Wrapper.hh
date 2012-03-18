#ifndef WRAPPER_H
#define WRAPPER_H
#include "Log.hh"

class Wrapper; //forward declare (Wrapper needs Job and Job needs Wrapper)
class File;

/*!
  \brief Class to write out the Wrapper and Wrapper steering file which will run the user's executable

  The wrapper script runs the user's executable. A different wrapper script will be used for different
  types of job to be run. For each Job within a Task, a different wrapper script will be generated, specifying
  the job's unique set of input/output files, output filenames, etc. The wrapper script itself will be the same
  for all of a task's jobs.

*/

class Wrapper {
public:
  Wrapper(const string myExecName, const string aType); ///<Type here means type of wrapper stored on database (set by -wrapName when registering script)
  virtual ~Wrapper()=0; ///<pABC
  virtual const string execName() const {return execName_;};
  virtual const string execNameFullHandle() const {return execNameFullHandle_;};
  virtual const set<File*>& files() const {return filesCreated_;};
  virtual const int save(string aDir); ///<Creates the files
protected:
  virtual int script();
  virtual int steer()=0; ///<sub-classes must override this function that does most of the work
  set<File*> filesCreated_; ///<Keeps track of any physical files created by object
  const string type_;
  string script_;
  string steer_;
private:
  const string execName_;
  string execNameFullHandle_;  
  //No implementation
  Wrapper(const Wrapper&);
  Wrapper& operator=(const Wrapper&);
};


#endif
