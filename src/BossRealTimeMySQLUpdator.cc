// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 1.0
// File:    BossRealTimeMySQLUpdator.cc
// Authors: Claudio Grandi (INFN BO)
// Date:    08/10/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include "BossConfiguration.h"

#include "BossRealTimeMySQLUpdator.h"
#include "BossOperatingSystem.h"

using namespace std;

BossRealTimeMySQLUpdator::BossRealTimeMySQLUpdator(int jobid) 
  : BossRealTimeUpdator(jobid), 
  last_err_time_(0), 
  current_connection_(0) {

  BossConfiguration* config=BossConfiguration::instance();
  hostname_    = config->boss_db_host();
  domain_      = config->boss_db_domain();
  user_        = config->boss_db_user();
  passwd_      = config->boss_db_user_pw();
  db_          = config->boss_db_name();
  unix_socket_ = config->boss_db_socket();
  port_        = config->boss_db_port();
  client_flag_ = config->boss_db_client_flag();
}

BossRealTimeMySQLUpdator::~BossRealTimeMySQLUpdator() {
}

// 1 job updated
// 0 job not updated
int BossRealTimeMySQLUpdator::updateJobParameter(int id, string tab, string key, string val) {
  int return_val=0;
  BossOperatingSystem* sys=BossOperatingSystem::instance();
  string idn = "JOBID";
  if (tab == "JOB") idn="ID";
  string query = "UPDATE "+tab+" SET "+key+"=\'"+val+
    "\' WHERE "+idn+"="+ sys->convert2string(id);
  //cerr << "BossRealTimeMySQLUpdator::updateJobParameter: " << query << endl;
  if(existColumn(key,tab)) {
    if (accept(query)==0)
      return_val=1;
  }
  return return_val;
}

char* BossRealTimeMySQLUpdator::parseInputString(string read) {
  char* return_val = 0;
  if ( read != "NULL" ) {
    return_val = new char[read.size()+1]; 
    strcpy(return_val,read.c_str());
  }
  return return_val;
}

bool BossRealTimeMySQLUpdator::connect() {
  bool ret_val = false;
  // avoid multiple connection
  if ( current_connection_ == 0 ) {
    current_connection_ = mysql_init(NULL);
    if ( !current_connection_ ) {
      cerr << "Out of memory" << endl;
      exit(800);
    } 
//      cout << "BossRealTimeMySQLUpdator::connect to: "
//  	 << " " << hostname_ 
//  	 << " " << user_ 
//  	 << " " << passwd_ 
//  	 << " " << db_ 
//  	 << endl;
    if ( mysql_real_connect(current_connection_, hostname_, user_, passwd_, 
			     db_, port_, unix_socket_, client_flag_) ) {
      ret_val = true;
    } else {
      fatal("connect",800);
      disconnect();
    }
  }
  return ret_val;
} 

bool BossRealTimeMySQLUpdator::disconnect() {
  if(current_connection_)mysql_close(current_connection_);
  current_connection_=0;
  return true;
}

// 0         succesfull
// -1        error in query
int BossRealTimeMySQLUpdator::tryQuery(MYSQL* conn, string q) {
  int ret = 0;
  int retry;
  do {
    retry = 0;
    if( mysql_query(current_connection_, q.c_str() ) ) {
      unsigned int errornumber = mysql_errno(current_connection_);
      switch ( errornumber ) {
      case 2013: {
	cerr << "Error in Boss/MySQL interface: " 
	     << mysql_error(current_connection_)
	     << " err_no " << mysql_errno(current_connection_) 
	     <<  endl;
	BossOperatingSystem* sys=BossOperatingSystem::instance();
	if ( sys->getTime()-last_err_time_ > 600 ) {
	  //disconnect();
	  //connect();
	  retry = 1;
	  cerr << "Try to make query again " << endl;
	  last_err_time_ = sys->getTime();
	} else {
	  cerr << "Consecutive errors. Abort. " << endl;
	  ret = -1;
	}
	break;
      }
      default:
	ret = -1;
      }
    }
  } while (retry);
  return ret;
}

void BossRealTimeMySQLUpdator::fatal(string routine, int exit_code) {

  cerr << "Fatal error in Boss/MySQL interface: " << routine << " --> " 
       << mysql_error(current_connection_)
       << " err_no " << mysql_errno(current_connection_) 
       << "\nplease check your MySQL server or contact the authors" <<  endl;
  exit(exit_code);  
}

MYSQL_RES* BossRealTimeMySQLUpdator::getResults(string q) {
  MYSQL_RES* return_val = 0;
  //    cout << q << endl;
  if ( tryQuery(current_connection_, q) ) {
    fatal("getResults (1) ",800);
    return return_val;
  }
  return_val = mysql_store_result(current_connection_);
  if ( !return_val ) {
    fatal("getResults (2) ",800);
    return return_val;
  }
  return return_val;
}

// 0         succesfull
// -1        error in query
int BossRealTimeMySQLUpdator::accept(string q) {
  //  cout << q << endl;
  if ( tryQuery(current_connection_, q) ) {
    fatal("accept (1) ",800);
    return -1;
  }
  return 0;
}

int BossRealTimeMySQLUpdator::clearResults(MYSQL_RES* r) {
  mysql_free_result(r);
  return 0;
}

int 
BossRealTimeMySQLUpdator::existColumn(string name, string table) {
  int return_val = 0;
  if (existTable(table)) {
    string query = string("SHOW FIELDS FROM ") + table;
    MYSQL_RES* current_results = getResults(query);
    if (current_results==0) {
      return return_val;
    }
    if ( mysql_num_rows(current_results) <=0 ) {
      cerr << "Wrong format" << endl;
      clearResults(current_results);
      return return_val;
    } 
    
    MYSQL_ROW row;
    while ( (row = mysql_fetch_row(current_results)) ) {
      string var=row[0];
      if (var == name) {
	return_val=1;
	break;
      }
    }
  }
  return return_val;
}

int BossRealTimeMySQLUpdator::existTable(string table) {
  int return_val = 0;
  string query = string("SHOW TABLES");
  MYSQL_RES* current_results = getResults(query);
  if (current_results==0) {
    return return_val;
  }
  if ( mysql_num_rows(current_results) <=0 ) {
    cerr << "Wrong format" << endl;
    clearResults(current_results);
    return return_val;
  } 

  MYSQL_ROW row;
  while ( (row = mysql_fetch_row(current_results)) ) {
    string tab=row[0];
    if (tab == table) {
      return_val=1;
      break;
    }
  }
  return return_val;
}
