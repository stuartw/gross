// /////////////////////////////////////////////////////////////////////
// Program: SIRDB
// Version: 1.0
// File:    MySQLBackendContact.cpp
// Authors: Claudio Grandi (INFN BO)
// Date:    28/11/2003
// Note:
// /////////////////////////////////////////////////////////////////////

#include "MySQLBackendContact.h"

#include "OperatingSystem.h"
#include "ClassAdLite.h"

#include <iostream>

MySQLBackendContact::MySQLBackendContact(const std::string& file, const std::string& mode) {

  //DEBUG
  //std::cerr << "MySQLBackendContact: reading configuration from " << file << std::endl;
  //ENDDEBUG
  CAL::ClassAdLite clad;
  int err = CAL::readFromFile(clad,file);
  if (err != 0) {
    std::cerr << "Unable to read configuration file: " << file
	      << std::endl;
    exit(301);
  }
  std::string tmp;
  //  MySQL server host name
  CAL::lookup(clad,"DB_HOST",tmp=""); CAL::removeOuterQuotes(tmp);
  db_host_     = OSUtils::string2char(tmp);
  //  MySQL server IP domain
  CAL::lookup(clad,"DB_DOMAIN",tmp=""); CAL::removeOuterQuotes(tmp);
  db_domain_   = OSUtils::string2char(tmp);
  //  MySQL database name
  CAL::lookup(clad,"DB_NAME",tmp="");  CAL::removeOuterQuotes(tmp);
  db_name_     = OSUtils::string2char(tmp);
  if(mode == "guest") {
    //  MySQL server guest user
    CAL::lookup(clad,"DB_GUEST",tmp=""); CAL::removeOuterQuotes(tmp);
    db_user_     = OSUtils::string2char(tmp);
    //  MySQL server guest user password
    CAL::lookup(clad,"DB_GUEST_PW",tmp=""); CAL::removeOuterQuotes(tmp);
    db_user_pw_  = OSUtils::string2char(tmp);
  } else {
    //  MySQL server main user
    CAL::lookup(clad,"DB_USER",tmp=""); CAL::removeOuterQuotes(tmp);
    db_user_    = OSUtils::string2char(tmp);
    //  MySQL server main user password
    CAL::lookup(clad,"DB_USER_PW",tmp=""); CAL::removeOuterQuotes(tmp);
    db_user_pw_ = OSUtils::string2char(tmp);
  }
  //  MySQL server port number
  CAL::lookup(clad,"DB_PORT",tmp="");
  db_port_     = OSUtils::string2int(tmp);
  //  MySQL server socket name
  CAL::lookup(clad,"DB_SOCKET",tmp=""); CAL::removeOuterQuotes(tmp);
  db_socket_   = OSUtils::string2char(tmp);
  //  MySQL server client flag
  CAL::lookup(clad,"DB_CLIENT_FLAG",tmp="");
  db_client_flag_ = OSUtils::string2int(tmp);
  //  MySQL server table type
  CAL::lookup(clad,"TABLE_TYPE",tmp=""); CAL::removeOuterQuotes(tmp);
  table_type_  = tmp;
  // DEBUG
  //dump();
  // END DEBUG
}

MySQLBackendContact::~MySQLBackendContact() {}

void
MySQLBackendContact::dump() {
  std::cout
    << "db_host " <<  db_host() << std::endl
    << "db_domain " <<  db_domain() << std::endl
    << "db_name " <<  db_name() << std::endl
    << "db_user " <<  db_user() << std::endl
    << "db_user_pw " <<  db_user_pw() << std::endl
    << "table_type " <<  db_table_type() << std::endl
    << "db_port " <<  db_port() << std::endl
    << "db_socket " <<  db_unix_socket() << std::endl
    << "db_client_flag " <<  db_client_flag() << std::endl
    ;
}
