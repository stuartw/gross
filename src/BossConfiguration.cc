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
#include "BossClassAd.h"

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
  BossClassAd* configad = 0;
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
  configad = new BossClassAd(boss_config_file_);
  // DEBUG
  //configad->dumpClad(cerr);
  // END DEBUG

  string read;
  configad->ClAdLookup( "BOSS_TOP_DIR", &boss_top_dir_);
  configad->ClAdLookup( "BOSS_VERSION", &boss_version_);
  configad->ClAdLookup( "BOSS_TMP_DIR", &boss_tmp_dir_);
  configad->ClAdLookup( "BOSS_UPD_INTERVAL", &read);
  boss_upd_interval_  = sys->string2int(read);
  configad->ClAdLookup( "BOSS_MAX_RETRY", &read);
  boss_max_retry_     = sys->string2int(read);
  configad->ClAdLookup( "BOSS_DB_HOST", &read);
  boss_db_host_       = sys->string2char(read);
  configad->ClAdLookup( "BOSS_DB_DOMAIN", &read);
  boss_db_domain_     = sys->string2char(read);
  configad->ClAdLookup( "BOSS_DB_NAME", &read);
  boss_db_name_       = sys->string2char(read);
  configad->ClAdLookup( "BOSS_DB_USER", &read);
  boss_db_user_       = sys->string2char(read);
  configad->ClAdLookup( "BOSS_DB_USER_PW", &read);
  boss_db_user_pw_    = sys->string2char(read);
  configad->ClAdLookup( "BOSS_DB_GUEST", &read);
  boss_db_guest_      = sys->string2char(read);
  configad->ClAdLookup( "BOSS_DB_GUEST_PW", &read);
  boss_db_guest_pw_   = sys->string2char(read);
  configad->ClAdLookup( "BOSS_TABLE_TYPE", &boss_table_type_);
  configad->ClAdLookup( "BOSS_DB_SOCKET", &read);
  boss_db_socket_     = sys->string2char(read);
  configad->ClAdLookup( "BOSS_DB_PORT", &read);
  boss_db_port_       = sys->string2int(read);
  configad->ClAdLookup( "BOSS_DB_CLIENT_FLAG", &read);
  boss_db_client_flag_= sys->string2int(read);
  
  delete configad;
  // DEBUG
  // dump();
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
