#ifndef ORCALOCJDL_H
#define ORCALOCJDL_H
#include "Log.hh"


#include "JDL.hh"
class CladLookup;
class OrcaLocJob; 

/*!
  \brief Creates JDL for an Orca type job.
*/

class OrcaLocJDL : public JDL {
public:
  OrcaLocJDL(const OrcaLocJob* aJob, CladLookup* aUserSpec, const string aFileName);
protected:
  const string script();
  const OrcaLocJob* job_;
  const CladLookup* userSpec_;
};


#endif
