#ifndef ORCAGWRAPPER_H
#define ORCAGWRAPPER_H
#include "Log.hh"

#include "Wrapper.hh"
class CladLookup;
class OrcaGWrapper; //forward declare (Wrapper needs Job and Job needs Wrapper)
class OrcaGJob; //forward declare (Wrapper needs Job and Job needs Wrapper)
class File;

/*!
  \brief Creates wrapper script and steering file for Orca Grid ("OrcaG") type jobs

*/

class OrcaGWrapper : public Wrapper {
public:
  OrcaGWrapper(const OrcaGJob* aJob, CladLookup* aUserSpec, const string myExecName, const string aType);
protected:
  int steer();
private:
  const OrcaGJob* job_;
  CladLookup* userSpec_; //note not const - reading a CladLookup changes the object!
};


#endif
