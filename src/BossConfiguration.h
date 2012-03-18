// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 1.0
// File:    BossConfiguration.cpp
// Authors: Claudio Grandi (INFN BO)
// Date:    28/11/2003
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_CONFIGURATION_H
#define BOSS_CONFIGURATION_H

#include <string>

class BossConfiguration {

private:
  static BossConfiguration* instance_;
  BossConfiguration();
  std::string boss_config_file_;
  std::string boss_top_dir_;
  std::string boss_version_;
  std::string boss_tmp_dir_;
  int boss_min_upd_int_;
  int boss_max_upd_int_;
  int boss_max_retry_;
  char* boss_db_host_;
  char* boss_db_domain_;
  char* boss_db_name_;
  char* boss_db_user_;
  char* boss_db_user_pw_;
  char* boss_db_guest_;
  char* boss_db_guest_pw_;
  std::string boss_table_type_;
  int boss_db_port_;
  char* boss_db_socket_;
  int boss_db_client_flag_;

public:

  // Constructor & Destructor
  // Nothing
  static BossConfiguration* instance();
  ~BossConfiguration();

  // Methods
  std::string boss_config_file(){ return boss_config_file_;}
  std::string boss_top_dir(){ return boss_top_dir_;}
  std::string boss_version(){ return boss_version_;}
  std::string boss_tmp_dir(){ return boss_tmp_dir_;}
  int boss_min_upd_int(){ return boss_min_upd_int_;}
  int boss_max_upd_int(){ return boss_max_upd_int_;}
  int boss_max_retry(){ return boss_max_retry_;}
  char* boss_db_host(){ return boss_db_host_;}
  char* boss_db_domain(){ return boss_db_domain_;}
  char* boss_db_name(){ return boss_db_name_;}
  char* boss_db_user(){ return boss_db_user_;}
  char* boss_db_user_pw(){ return boss_db_user_pw_;}
  char* boss_db_guest(){ return boss_db_guest_;}
  char* boss_db_guest_pw(){ return boss_db_guest_pw_;}
  std::string boss_table_type(){ return boss_table_type_;}
  int boss_db_port(){ return boss_db_port_;}
  char* boss_db_socket(){ return boss_db_socket_;}
  int boss_db_client_flag(){ return boss_db_client_flag_;}

  void dump();
};

#endif
