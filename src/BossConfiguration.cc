// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 1.0
// File:    BossConfiguration.cpp
// Authors: Claudio Grandi (INFN BO)
// Date:    28/11/2003
// Note:
// /////////////////////////////////////////////////////////////////////

#include "BossConfiguration.h"

#include "BossOperatingSystem.h"
#include "ClassAdLite.h"

#include <iostream>

using namespace std;

BossConfiguration* BossConfiguration::instance_ = 0;

BossConfiguration* BossConfiguration::instance() {
  if(!instance_)
    instance_ = new BossConfiguration();
  return instance_;
}

BossConfiguration::BossConfiguration() {

  BossOperatingSystem* sys=BossOperatingSystem::instance();

  // look for configuration file
  string bossTop = sys->getEnv("BOSSDIR");
  string configFile = "BossConfig.clad";
  string configFilePath = ".";           // look in current directory
  //  cout << "checking " << configFilePath+"/"+configFile << endl;
  if ( !sys->fileExist(configFilePath+"/"+configFile) ) { 
    configFilePath = sys->getEnv("HOME");
    //    cout << "checking " << configFilePath+"/"+configFile << endl;
    if ( !sys->fileExist(configFilePath+"/"+configFile) ) { 
      configFilePath = sys->getEnv("BOSSDIR");;
      //      cout << "checking " << configFilePath+"/"+configFile << endl;
      if ( !sys->fileExist(configFilePath+"/"+configFile) ) { 
	cerr << "configuration ClassAd file doesn't exist" << endl;
	//	return -10;
      }
    }
  }
  boss_config_file_ = configFilePath+"/"+configFile;
  //DEBUG
  //cout << "BossConfiguration: reading configuration from " << boss_config_file_ << endl;
  //ENDDEBUG
  CAL::ClassAdLite clad;
  int err = CAL::readFromFile(clad,boss_config_file_);
  if (err != 0) {
    std::cerr << "Unable to read configuration file: " << boss_config_file_
	      << std::endl;
    exit(301);
  }
  std::string tmp;
  // BOSS top directory
  CAL::lookup(clad,"BOSS_TOP_DIR",tmp=""); CAL::removeOuterQuotes(tmp);
  boss_top_dir_     = tmp;
  // BOSS version
  CAL::lookup(clad,"BOSS_VERSION",tmp=""); CAL::removeOuterQuotes(tmp);
  boss_version_     = tmp;
  // BOSS temporary directory
  CAL::lookup(clad,"BOSS_TMP_DIR",tmp=""); CAL::removeOuterQuotes(tmp);
  boss_tmp_dir_     = tmp;
  // Time interval between two consecutive updates to BOSS DB
  CAL::lookup(clad,"BOSS_UPD_INTERVAL",tmp="");
  boss_upd_interval_= sys->string2int(tmp);
  // Maximum number of retries before killing dbUpdator if it doesn't exit
  CAL::lookup(clad,"BOSS_MAX_RETRY",tmp="");
  boss_max_retry_   = sys->string2int(tmp);
  // BOSS MySQL server host name
  CAL::lookup(clad,"BOSS_DB_HOST",tmp=""); CAL::removeOuterQuotes(tmp);
  boss_db_host_     = sys->string2char(tmp);
  // BOSS MySQL server IP domain
  CAL::lookup(clad,"BOSS_DB_DOMAIN",tmp=""); CAL::removeOuterQuotes(tmp);
  boss_db_domain_   = sys->string2char(tmp);
  // BOSS MySQL database name
  CAL::lookup(clad,"BOSS_DB_NAME",tmp="");  CAL::removeOuterQuotes(tmp);
  boss_db_name_     = sys->string2char(tmp);
  // BOSS MySQL server main user
  CAL::lookup(clad,"BOSS_DB_USER",tmp=""); CAL::removeOuterQuotes(tmp);
  boss_db_user_     = sys->string2char(tmp);
  // BOSS MySQL server main user password
  CAL::lookup(clad,"BOSS_DB_USER_PW",tmp=""); CAL::removeOuterQuotes(tmp);
  boss_db_user_pw_  = sys->string2char(tmp);
  // BOSS MySQL server guest user
  CAL::lookup(clad,"BOSS_DB_GUEST",tmp=""); CAL::removeOuterQuotes(tmp);
  boss_db_guest_    = sys->string2char(tmp);
  // BOSS MySQL server guest user password
  CAL::lookup(clad,"BOSS_DB_GUEST_PW",tmp=""); CAL::removeOuterQuotes(tmp);
  boss_db_guest_pw_ = sys->string2char(tmp);
  // BOSS MySQL server port number
  CAL::lookup(clad,"BOSS_DB_PORT",tmp="");
  boss_db_port_     = sys->string2int(tmp);
  // BOSS MySQL server socket name
  CAL::lookup(clad,"BOSS_DB_SOCKET",tmp=""); CAL::removeOuterQuotes(tmp);
  boss_db_socket_   = sys->string2char(tmp);
  // BOSS MySQL server client flag
  CAL::lookup(clad,"BOSS_DB_CLIENT_FLAG",tmp="");
  boss_db_client_flag_ = sys->string2int(tmp);
  // BOSS MySQL server table type
  CAL::lookup(clad,"BOSS_TABLE_TYPE",tmp=""); CAL::removeOuterQuotes(tmp);
  boss_table_type_  = tmp;
  // DEBUG
  //dump();
  // END DEBUG
}

BossConfiguration::~BossConfiguration() {}

void
BossConfiguration::dump() {
  cout
    << "boss_config_file " <<  boss_config_file() << endl
    << "boss_top_dir " <<  boss_top_dir() << endl
    << "boss_version " <<  boss_version() << endl
    << "boss_tmp_dir " <<  boss_tmp_dir() << endl
    << "boss_upd_interval " <<  boss_upd_interval() << endl
    << "boss_max_retry " <<  boss_max_retry() << endl
    << "boss_db_host " <<  boss_db_host() << endl
    << "boss_db_domain " <<  boss_db_domain() << endl
    << "boss_db_name " <<  boss_db_name() << endl
    << "boss_db_user " <<  boss_db_user() << endl
    << "boss_db_user_pw " <<  boss_db_user_pw() << endl
    << "boss_db_guest " <<  boss_db_guest() << endl
    << "boss_db_guest_pw " <<  boss_db_guest_pw() << endl
    << "boss_table_type " <<  boss_table_type() << endl
    << "boss_db_port " <<  boss_db_port() << endl
    << "boss_db_socket " <<  boss_db_socket() << endl
    << "boss_db_client_flag " <<  boss_db_client_flag() << endl
    ;
}
