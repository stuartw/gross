#ifndef ORCAJDL_H
#define ORCAJDL_H
#include "Log.hh"


#include "JDL.hh"
class CladLookup;
class OrcaGJob; 

/*!
  \brief Creates JDL for an Orca type job.
*/

class OrcaJDL : public JDL {
public:
  OrcaJDL(const OrcaGJob* aJob, CladLookup* aUserSpec, const string aFileName);
protected:
  const string script();
  const OrcaGJob* job_;
  CladLookup* userSpec_;
};


#endif
