#ifndef ORCALOCWRAPPER_H
#define ORCALOCWRAPPER_H
#include "Log.hh"

#include "Wrapper.hh"
class CladLookup;
class OrcaLocWrapper; //forward declare (Wrapper needs Job and Job needs Wrapper)
class OrcaLocJob; //forward declare (Wrapper needs Job and Job needs Wrapper)
class File;

/*!
  \brief Creates wrapper script and steering file for Orca Grid ("OrcaG") type jobs

*/

class OrcaLocWrapper : public Wrapper {
public:
  OrcaLocWrapper(const OrcaLocJob* aJob, CladLookup* aUserSpec, const string myExecName, const string aType);
protected:
  int steer();
private:
  const OrcaLocJob* job_;
  CladLookup* userSpec_; //note not const - reading a CladLookup changes the object!
};


#endif
