// /////////////////////////////////////////////////////////////////////
// Program: SIRDB
// Version: 1.0
// File:    ClarensBackendContact.cpp
// Authors: Claudio Grandi (INFN BO)
// Date:    28/11/2003
// Note:
// /////////////////////////////////////////////////////////////////////

#include "ClarensBackendContact.h"

#include "ClassAdLite.h"

#include <iostream>

ClarensBackendContact::ClarensBackendContact(const std::string& file) {

  //DEBUG
  //cout << "ClarensBackendContact: reading configuration from " << file << endl;
  //ENDDEBUG
  CAL::ClassAdLite clad;
  int err = CAL::readFromFile(clad,file);
  if (err != 0) {
    std::cerr << "Unable to read configuration file: " << file
	      << std::endl;
    exit(301);
  }
  std::string tmp;
  //  Clarens URL
  CAL::lookup(clad,"CLARENS_URL",tmp=""); CAL::removeOuterQuotes(tmp);
  clarens_url_     = OSUtils::string2char(tmp);
  //  Clarens Certificate
  CAL::lookup(clad,"CLARENS_CERT",tmp=""); CAL::removeOuterQuotes(tmp);
   clarens_cert_ = tmp;
  //  Clarens Key
  CAL::lookup(clad,"CLARENS_KEY",tmp=""); CAL::removeOuterQuotes(tmp);
  clarens_key_  = tmp;
  //  Clarens proxy
  CAL::lookup(clad,"CLARENS_PROXY",tmp=""); CAL::removeOuterQuotes(tmp);
  clarens_proxy_  = tmp;
  // DEBUG
  //dump();
  // END DEBUG
}

ClarensBackendContact::~ClarensBackendContact() {}

void
ClarensBackendContact::dump() {
  std::cout
    << "URL " <<  clarens_url() << std::endl
    << "cert " <<  clarens_cert() << std::endl
    << "key " <<  clarens_key() << std::endl
    << "proxy " <<  clarens_proxy() << std::endl
    ;
}
