// /////////////////////////////////////////////////////////////////////
// Program: SirDB
// Version: 1.0
// File:    SirDBBackend.cc
// Authors: Jordan Carlson (Caltech), Claudio Grandi (INFN BO)
// Date:    16/09/2004
// Note:
// /////////////////////////////////////////////////////////////////////

#include "SirDBBackend.h"

#ifdef CLARENS_BACKEND

#include "ClarensBackend.h"
SirDBBackend* SirDBBackend::create(const std::string& config, const std::string& mode) {
  return new ClarensBackend(config, mode);
}

#else // default to MySQL

#include "MySQLBackend.h"
SirDBBackend* SirDBBackend::create(const std::string& config, const std::string& mode) {
  return new MySQLBackend(config, mode);
}

#endif
