#ifndef JDL_H
#define JDL_H
#include "Log.hh"

class CladLookup;
class JDL; 
class Job; 


class JDL {
public:
  JDL(const Job* aJob, const CladLookup* aUserSpec, const string aFileName);
  const int save(string aDir);
  const string name() const {return name_;};
  const string fullHandle() const {return fullHandle_;};
private:
  const Job* job_;
  const CladLookup* userSpec_;
  const string script();
  const string name_;
  string fullHandle_;
  //No implementation
  JDL(const JDL&);
  JDL& operator=(const JDL&);
};


#endif
