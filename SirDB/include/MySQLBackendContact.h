// /////////////////////////////////////////////////////////////////////
// Program: SIRDB
// Version: 1.0
// File:    MySQLBackendContact.cpp
// Authors: Claudio Grandi (INFN BO)
// Date:    28/11/2003
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef MYSQL_BACKEND_CONTACT_H
#define MYSQL_BACKEND_CONTACT_H

#include <string>

class MySQLBackendContact {

private:
  char* db_host_;
  char* db_domain_;
  char* db_name_;
  char* db_user_;
  char* db_user_pw_;
  std::string table_type_;
  int db_port_;
  char* db_socket_;
  int db_client_flag_;

public:

  // Constructor & Destructor
  // Nothing
  MySQLBackendContact(const std::string&, const std::string&);
  ~MySQLBackendContact();

  // Methods
  char* db_host(){ return db_host_;}
  char* db_domain(){ return db_domain_;}
  char* db_name(){ return db_name_;}
  char* db_user(){ return db_user_;}
  char* db_user_pw(){ return db_user_pw_;}
  std::string db_table_type(){ return table_type_;}
  int db_port(){ return db_port_;}
  char* db_unix_socket(){ return db_socket_;}
  int db_client_flag(){ return db_client_flag_;}

  void dump();
};

#endif
