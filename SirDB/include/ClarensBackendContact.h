// /////////////////////////////////////////////////////////////////////
// Program: SIRDB
// Version: 1.0
// File:    ClarensBackendContact.cpp
// Authors: Claudio Grandi (INFN BO)
// Date:    28/11/2003
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef CLARENS_BACKEND_CONTACT_H
#define CLARENS_BACKEND_CONTACT_H

#include <string>

class ClarensBackendContact {

private:
  char* clarens_url_;
  std::string clarens_cert_;
  std::string clarens_key_;
  std::string clarens_proxy_;

public:

  // Constructor & Destructor
  // Nothing
  ClarensBackendContact(const std::string&);
  ~ClarensBackendContact();

  // Methods
  char* clarens_url() { return clarens_url_; }
  std::string clarens_cert() { return clarens_cert_; }
  std::string clarens_key() { return clarens_key_; }
  std::string clarens_proxy() { return clarens_proxy_; }

  void dump();
};

#endif
