#ifndef EDGIF_H
#define EDGIF_H

#include "Log.hh"

/*! \brief EDG Interface class
  Implemented as a class for future scalability. Not strictly necessary at the moment.
*/

class EDGIf {
public:
  EDGIf() {};
  string getOutput(const string edgJobId, string oDir="./") const;
private: 
  //No implementation
  EDGIf(const EDGIf&);
  EDGIf operator=(const EDGIf&);
};

#endif
