// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossJobelement.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:    
// /////////////////////////////////////////////////////////////////////

#include "BossJobElement.h"

using namespace std;

ostream& operator<< (ostream& os, const BossJobElement& e) {
  os << "(" << e.name_ << ":" << e.type_ << "=" << e.value_ << ")";
  return os;
}

