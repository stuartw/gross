#ifndef JDL_H
#define JDL_H
#include "Log.hh"

/*!
  \brief Class to write out a JDL for BOSS to use for submission

  BOSS requires a JDL to specify its submission parameters - no matter what type
  of queue BOSS submits to. This class will create this file.

  This pABC defines a standard interface for a JDL. Concrete classes derived from this
  class will implement the script() function that actually writes out the contents of the
  JDL file that is created.

*/

class JDL {
public:
  JDL(const string aFileName);
  virtual ~JDL()=0; //pABC
  virtual int save(string aDir);
  virtual const string name() const {return name_;};
  virtual const string fullHandle() const {return fullHandle_;};
protected:
  virtual const string script()=0; //must be defined in sub-classes
private:
  const string name_;
  string fullHandle_;
  //No implementation
  JDL(const JDL&);
  JDL& operator=(const JDL&);
};


#endif
