// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossDatabase.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include "BossDatabase.h"

#include "BossConfiguration.h"

#include <iostream>
#include <sstream>

#include "BossOperatingSystem.h"
#include "BossJob.h"
#include "BossUpdateElement.h"

using namespace std;

BossDatabase::BossDatabase(string mode) : 
  mode_(mode), last_err_time_(0), current_connection_(0) {
  
  BossConfiguration* config=BossConfiguration::instance();
  hostname_    = config->boss_db_host();
  domain_      = config->boss_db_domain();
  super_user_  = config->boss_db_user();
  guest_user_  = config->boss_db_guest();
  super_passwd_= config->boss_db_user_pw();
  guest_passwd_= config->boss_db_guest_pw();
  db_          = config->boss_db_name();
  unix_socket_ = config->boss_db_socket();
  port_        = config->boss_db_port();
  client_flag_ = config->boss_db_client_flag();
  table_type_  = config->boss_table_type();
  
  connect();
}


BossDatabase::~BossDatabase() {
  disconnect();
}

int BossDatabase::connect() {

  // avoid multiple connection
  if ( current_connection_ == 0 ) {
    current_connection_ = mysql_init(NULL);
    if ( !current_connection_ ) {
      cerr << "Out of memory" << endl;
      exit(800);
    } 
    char* user;
    char* pw;
    // DEBUG
    // cout << "BossDatabase is connecting as " << mode_ << " user" << endl;
    // END DEBUG
    if ( mode_ == "super" ) { 
      user = super_user_;
      pw   = super_passwd_;
    } else {
      user = guest_user_;
      pw   = guest_passwd_;
    }
    if ( !mysql_real_connect(current_connection_, hostname_, user, pw, db_,
			     port_, unix_socket_, client_flag_) ) {
      fatal("connect",800);
      disconnect();
    }
  }
  return 1;
} 

int BossDatabase::disconnect() {
  // DEBUG
  // cout << "BossDatabase is disconnecting " << mode_ << " user" << endl;
  // END DEBUG
  if(current_connection_)mysql_close(current_connection_);
  current_connection_=0;
  return 0;
}

// 0         succesfull
// -1        error in query
int BossDatabase::tryQuery(MYSQL* conn, string q) {
  int ret = 0;
  int retry;
  do {
    retry = 0;
    if( mysql_query(current_connection_, q.c_str() ) ) {
      unsigned int errornumber = mysql_errno(current_connection_);
      switch ( errornumber ) {
      case 2013: {
	cout << "Error in Boss/MySQL interface: " 
	     << mysql_error(current_connection_)
	     << " err_no " << mysql_errno(current_connection_) 
	     <<  endl;
	BossOperatingSystem* sys=BossOperatingSystem::instance();
	if ( sys->getTime()-last_err_time_ > 600 ) {
	  //disconnect();
	  //connect();
	  retry = 1;
	  cout << "Try to make query again " << endl;
	  last_err_time_ = sys->getTime();
	} else {
	  cout << "Consecutive errors. Abort. " << endl;
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

void BossDatabase::fatal(string routine, int exit_code) {

  cout << "Fatal error in Boss/MySQL interface: " << routine << " --> " 
       << mysql_error(current_connection_)
       << " err_no " << mysql_errno(current_connection_) 
       << "\nplease check your MySQL server or contact the authors" <<  endl;
  exit(exit_code);  
}

MYSQL_RES* BossDatabase::getResults(string q) {
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
int BossDatabase::accept(string q) {
  //  cout << q << endl;
  if ( tryQuery(current_connection_, q) ) {
    fatal("accept (1) ",800);
    return -1;
  }
  return 0;
}

int BossDatabase::clearResults(MYSQL_RES* r) {
  mysql_free_result(r);
  return 0;
}

// >= 0       succesfull
// -1         error
int BossDatabase::affectedRows() {

  if (current_connection_) 
    return mysql_affected_rows(current_connection_);
  else 
    return -1;
}

// 1         the job exist
// 0         the job doesn't exist or there was an error
int BossDatabase::existJob(int id) {

  int return_val = 0;
  
  BossOperatingSystem* sys=BossOperatingSystem::instance();

  string query = string("SELECT ID FROM JOB WHERE ID =") + sys->convert2string(id);
  MYSQL_RES* current_results = getResults(query);
  if (current_results==0) {
    return return_val;
  }

  if ( mysql_num_rows(current_results) > 0 ) 
    return_val = 1;

  clearResults(current_results);

  return return_val;
}

// 1         the job type exist
// 0         the job type doesn't exist or there was an error
int BossDatabase::existJobType(string job_type) {

  int return_val = 0;

  string query = "SELECT name FROM JOBTYPE WHERE NAME =\'" + job_type + "\'";
  MYSQL_RES* current_results = getResults(query);
  if (current_results==0) {
    return return_val;
  }

  if ( mysql_num_rows(current_results) > 0 ) 
    return_val = 1;

  clearResults(current_results);

  return return_val;
}

// 1        the job exist
// 0        the job doesn't exist or there was an error
int BossDatabase::existSchType(string sch) {

  int return_val = 0;

  string query = "SELECT * FROM SCHEDULER WHERE NAME =\'" + sch + "\'";;
  MYSQL_RES* current_results = getResults(query);
  if (current_results==0) {
    return return_val;
  }
  if ( mysql_num_rows(current_results) > 0 ) 
    return_val = 1;
 
  clearResults(current_results);

  return return_val;
}

// return "NULL" if there was an error default scheduler
// -1       there was an error
string BossDatabase::getDefaultSch() {

  string sch = "NULL";

  string query = "SELECT name FROM SCHEDULER WHERE def =\'Y\'";
  MYSQL_RES* current_results = getResults(query);
  if (current_results==0) {
    return sch;
  }

  if ( mysql_num_rows(current_results) != 1 ) {
    cout << "DB inconsistency !! there is no default scheduler !!" << endl;
    clearResults(current_results);
    return sch;
  } 
 
  MYSQL_ROW row = mysql_fetch_row(current_results);
  sch = string(row[0]);

  clearResults(current_results);

  return sch;
}

  

string BossDatabase::getWorkDir(string name) {

  string return_val = "";

  string query = "SELECT TOP_WORK_DIR FROM SCHEDULER WHERE NAME =\'" + name + "\'";;
  MYSQL_RES* current_results = getResults(query);
  if (current_results==0) {
    return return_val;
  }

  if ( mysql_num_rows(current_results) != 1 ) {
    cout << "DB inconsistency !! Top working dir not set for scheduler !!" << endl;
    clearResults(current_results);
    return return_val;
  } 
 
  MYSQL_ROW row = mysql_fetch_row(current_results);
  return_val = string(row[0]);
 
  clearResults(current_results);

  return return_val;

}

// 1 job inserted
// 0 job not inserted
int BossDatabase::insertJob(BossJob* jobH) {

  // add a new job and return the unique job handle
  int return_val=0;

  BossOperatingSystem* sys=BossOperatingSystem::instance();

  string names = "";
  string values = "";
  BossJob::const_iterator i;
  for (i=jobH->beginGeneral(); i!= jobH->endGeneral(); i++ ) {
    string n = jobH->generalData(i).name();
    string v = jobH->generalData(i).value();
    string t = jobH->generalData(i).type();
    if(n != "ID" && v != "" ) {
      string delimiter = "\'";
      //      if (t == "int" || t == "INT" )delimiter = "";
      names += n + ",";
      values += delimiter + v + delimiter + ",";
    }
  }
  string query = "INSERT INTO JOB(" + names.substr(0,names.size()-1) +
                 ") VALUES ("       + values.substr(0,values.size()-1) + ")";
  if (accept(query)!=0) {
    return return_val;
  }

  // get the unique id
  int jobID = -1;
  if ( mysql_affected_rows(current_connection_) >= 1 ) 
    jobID = mysql_insert_id(current_connection_);

  jobH->setId(jobID);

  // create job specific entries
  vector<string> types = jobH->getJobTypes();
  for(vector<string>::const_iterator ti=types.begin(); ti<types.end(); ti++) {
    if ( *ti != "stdjob" ) {
      string names = "ID,JOBID";
      string values = string("NULL,") + sys->convert2string(jobID);
      BossJob::const_iterator i;
      for (i=jobH->beginSpecific(); i!= jobH->endSpecific(); i++ ) {
	string n = jobH->specificData(i).name();
	if(jobH->existColumn((*ti),n)) {
	  string v = jobH->specificData(i).value();
	  string t = jobH->specificData(i).type();
	  string delimiter = "\'";
	  //      if (t == "int" || t == "INT" ) delimiter = "";
	  names += "," + n;
	  values += "," + delimiter + v + delimiter;
	}
      }
      string query = "INSERT INTO "+(*ti)+"(" + names +
	") VALUES ("       + values + ")";
      if (accept(query)!=0) {
	return return_val;
      }
    }
  }

  return 1;
}

// 0 successful
// <0 error
int BossDatabase::deleteJob(int jobID) {
  
  int return_val = -1;

  BossOperatingSystem* sys=BossOperatingSystem::instance();

  string query = string("DELETE FROM JOB WHERE ID = ") + sys->convert2string(jobID);
  if (accept(query)!=0) {
      return return_val;
  }
  
  if ( affectedRows() <= 0 ) { // the job is not found 
    return return_val;
  } 
        
  return 0;
} 

// 0 successful
// <0 error
int BossDatabase::deleteSpecificJob(string type, int jobID) {

  int return_val = -1;

  BossOperatingSystem* sys=BossOperatingSystem::instance();

  string query = string("DELETE FROM ") + type + " WHERE JOBID = " + sys->convert2string(jobID);
 
  if (accept(query)!=0) {
      return return_val;
  }

  if ( affectedRows()  <= 0 ) { // the job is not found 
    return return_val;
  } 

  return 0;
}

// 0 successful
// <0 error
int BossDatabase::getGeneralJobData(BossJob* jobH) {

  int return_val = -1;

  BossOperatingSystem* sys=BossOperatingSystem::instance();

  if (!jobH)return return_val;
  
  int id = jobH->getId();
  if (id <= 0) return return_val;
  string strid = sys->convert2string(id);
  string query = string("SELECT * FROM JOB WHERE ID = ") + strid;
  MYSQL_RES* current_results = getResults(query);
  if (current_results==0) {
    return return_val;
  }
  
  if ( mysql_num_rows(current_results) <= 0 ) { // the job is not found 
    clearResults(current_results);
    return return_val;
  } 
        
  unsigned int num_fields = mysql_num_fields(current_results);
  MYSQL_FIELD* fields = mysql_fetch_fields(current_results);
  MYSQL_ROW row = mysql_fetch_row(current_results);
  jobH->setId(atoi(row[0]));
  for (unsigned int i=1; i!= num_fields; i++) {
    string n = fields[i].name;
    string v = string(row[i]);
    jobH->setData(BossUpdateElement(id,"JOB",fields[i].name,string(row[i])));
  }
  
  clearResults(current_results);

  return 0;

}


// 0 successful
// <0 error
int BossDatabase::getSpecificJobData(BossJob* jobH) {

  int return_val = -1;

  BossOperatingSystem* sys=BossOperatingSystem::instance();

  if (!jobH)return return_val;

  int id = jobH->getId();
  if (id <= 0) return return_val;
  string strid = sys->convert2string(id);
  vector<string> types = jobH->getJobTypes();
  for(vector<string>::const_iterator ti=types.begin(); ti<types.end(); ti++) {
    if ( *ti != "stdjob" ) {
      if ( existJobType(*ti) ) {
	string query = string("SELECT * FROM "+(*ti)+" WHERE JOBID = ") + strid;
	MYSQL_RES* current_results = getResults(query);
	if (current_results==0) {
	  return return_val;
	}
      
	if ( mysql_num_rows(current_results) <= 0 ) {
	  // the job is not found  
	  clearResults(current_results);
	  return return_val;
	} 
      
	unsigned int num_fields = mysql_num_fields(current_results);
	MYSQL_FIELD* fields = mysql_fetch_fields(current_results);
	MYSQL_ROW row = mysql_fetch_row(current_results);
	for (unsigned int i=0; i!= num_fields; i++) {
	  string n = fields[i].name;
	  string v = string(row[i]);
	  //      cout << "***" << n << " " << v << endl;
	  if(n!="ID" && n != "JOBID") {
	    jobH->setData(BossUpdateElement(id,*ti,fields[i].name,string(row[i])));
	  }
	}
      
	clearResults(current_results);

      }
    } else {
      cerr << "Job type " << (*ti) << " does not exisit! " << endl;
    }
  }

  return 0;

}

// 0 successful
// <0 error
// >0 file not stored
int BossDatabase::getSubmit(string name, string fname) {

  string query;
  string blob = "";

  query = "SELECT SUBMIT_BIN FROM SCHEDULER WHERE NAME = \"" + name + "\""; 
  MYSQL_RES* current_results = getResults(query);
  if (current_results==0) {
    return -1;
  }
  
  if ( mysql_num_rows(current_results) <= 0 ) { // the scheduler type is not found 
    clearResults(current_results);
    return -2;
  } 
  
  MYSQL_ROW row = mysql_fetch_row(current_results);
  blob = string(row[0]);

  clearResults(current_results);
  if(blob == "NULL" || blob == "")
    return 1;

  BossOperatingSystem* sys=BossOperatingSystem::instance();

  ofstream f(fname.c_str());
  f << blob; 
  f.close();
  // change privileges
  sys->fileChmod("744",fname);
  
  return 0;
}

// 0 successful
// <0 error
// >0 file not stored
int BossDatabase::getKill(string name, string fname) {

  string query;
  string blob = "";

  query = "SELECT KILL_BIN FROM SCHEDULER WHERE NAME = \"" + name + "\""; 
  MYSQL_RES* current_results = getResults(query);
  if (current_results==0) {
    return -1;
  }
  
  if ( mysql_num_rows(current_results) <= 0 ) { // the scheduler type is not found 
    clearResults(current_results);
    return -2;
  } 
  
  MYSQL_ROW row = mysql_fetch_row(current_results);
  blob = string(row[0]);

  clearResults(current_results);
  if(blob == "NULL" || blob == "")
    return 1;

  BossOperatingSystem* sys=BossOperatingSystem::instance();

  ofstream f(fname.c_str());
  f << blob; 
  f.close();
  // change privileges
  sys->fileChmod("744",fname);
  
  return 0;
}

// 0 successful
// <0 error
// >0 file not stored
int BossDatabase::getQuery(string name, string fname) {

  string query;
  string blob = "";

  query = "SELECT QUERY_BIN FROM SCHEDULER WHERE NAME = \"" + name + "\""; 
  MYSQL_RES* current_results = getResults(query);
  if (current_results==0) {
    return -1;
  }
  
  if ( mysql_num_rows(current_results) <= 0 ) { // the scheduler type is not found 
    clearResults(current_results);
    return -2;
  } 
  
  MYSQL_ROW row = mysql_fetch_row(current_results);
  blob = string(row[0]);

  clearResults(current_results);
  if(blob == "NULL" || blob == "")
    return 1;

  BossOperatingSystem* sys=BossOperatingSystem::instance();

  ofstream f(fname.c_str());
  f << blob; 
  f.close();
  // change privileges
  sys->fileChmod("744",fname);
  
  return 0;
}

// 0 successful
// <0 error
// >0 file not stored
int BossDatabase::getPreProcess(string type, string fname) {

  string query;
  string blob = "";

  query = "SELECT PRE_BIN FROM JOBTYPE WHERE NAME = \"" + type + "\""; 
  MYSQL_RES* current_results = getResults(query);
  if (current_results==0) {
    return -1;
  }
  
  if ( mysql_num_rows(current_results) <= 0 ) { // the scheduler type is not found 
    clearResults(current_results);
    return -2;
  } 
  
  MYSQL_ROW row = mysql_fetch_row(current_results);
  blob = string(row[0]);

  clearResults(current_results);
  if(blob == "NULL" || blob == "")
    return 1;
  
  BossOperatingSystem* sys=BossOperatingSystem::instance();

  ofstream f(fname.c_str());
  f << blob; 
  f.close();
  // change privileges
  sys->fileChmod("744",fname);

  return 0;
}

int BossDatabase::getRuntimeProcess(string type, string fname) {

  string query;
  string blob = "";

  query = "SELECT RUN_BIN FROM JOBTYPE WHERE NAME = \"" + type + "\""; 
  MYSQL_RES* current_results = getResults(query);
  if (current_results==0) {
    return -1;
  }
  
  if ( mysql_num_rows(current_results) <= 0 ) { // the scheduler type is not found 
    clearResults(current_results);
    return -2;
  } 
  
  MYSQL_ROW row = mysql_fetch_row(current_results);
  blob = string(row[0]);

  clearResults(current_results);
  if(blob == "NULL" || blob == "")
    return 1;

  BossOperatingSystem* sys=BossOperatingSystem::instance();

  ofstream f(fname.c_str());
  f << blob; 
  f.close();
  // change privileges
  sys->fileChmod("744",fname);
  
  return 0;
}

int BossDatabase::getPostProcess(string type, string fname) {

  string query;
  string blob = "";

  query = "SELECT POST_BIN FROM JOBTYPE WHERE NAME = \"" + type + "\""; 
  MYSQL_RES* current_results = getResults(query);
  if (current_results==0) {
    return -1;
  }
  
  if ( mysql_num_rows(current_results) <= 0 ) { // the scheduler type is not found 
    clearResults(current_results);
    return -2;
  } 
  
  MYSQL_ROW row = mysql_fetch_row(current_results);
  blob = string(row[0]);

  clearResults(current_results);
  if(blob == "NULL" || blob == "")
    return 1;

  BossOperatingSystem* sys=BossOperatingSystem::instance();

  ofstream f(fname.c_str());
  f << blob; 
  f.close();
  // change privileges
  sys->fileChmod("744",fname);
 
  return 0;
}

// return 0 if the job was not found
BossJob* BossDatabase::findJob(int jobID) {

  BossJob* jobH = new BossJob();
  jobH->setId(jobID);

  // General part
  if(getGeneralJobData(jobH)!=0)return 0;

  // Specific part
  vector<string> types = jobH->getJobTypes();
  for(vector<string>::const_iterator ti=types.begin(); ti<types.end(); ti++) {
    if ( *ti != "stdjob" ) {
      if ( existJobType(*ti) ) {
	string schema = findSchema(*ti);
	std::stringstream sch;
	sch << schema;
	jobH->setSchema(*ti,sch);
	if(getSpecificJobData(jobH)!=0)return 0;
      } else {
	cerr << "Job type " << (*ti) << " does not exisit! " << endl;
      }
    }
  }

  // Set the default update mode to be the database for future operations
  jobH->setUpdator(this);

  return jobH;
}

// 1 job updated
// 0 job not updated
int BossDatabase::updateJobParameter(int id, string tab, string key, string val) {
  int return_val=0;

  BossOperatingSystem* sys=BossOperatingSystem::instance();

  if(existColumn(key,tab)) {
    string idn = "JOBID";
    if (tab == "JOB") idn="ID";
    string query = "UPDATE "+tab+" SET "+key+"=\'"+val+
      "\' WHERE "+idn+"="+ sys->convert2string(id);
    if (accept(query)==0)
      return_val=1;
  }
  return return_val;
}

// 0 succesful
// -1 error
int BossDatabase::registerScheduler (string name, string defs, string topwdirs,
				     string submit_file, string kill_file, string query_file) {

  int return_val = -1;

  // check if exist
  int update_mode=0;
  int was_default=0;
  string query = "SELECT * FROM SCHEDULER WHERE NAME =\'" + name + "\'";;
  MYSQL_RES* current_results = getResults(query);
  if (current_results==0) {
    return return_val;
  }
  if ( mysql_num_rows(current_results) >=1 ) {
    update_mode=1;
    MYSQL_ROW row = mysql_fetch_row(current_results);
    if ( string(row[1]) == "Y") {
      was_default=1;
    } 
  }

  BossOperatingSystem* sys=BossOperatingSystem::instance();

  // Check consistency of "default" attribute
  string def = (defs=="TRUE") ? "Y" : "N";
  if         (!was_default && def=="Y") { 
    query = string("UPDATE SCHEDULER SET ")
      + "DEF = \'N\'"
      + " WHERE DEF = \'Y\'";
    if (accept(query)!=0) {
      return return_val;
    }
    cerr << name << " is the now the default scheduler!" << endl;
  } else if (was_default && def=="N") { 
    def = "Y";
  }

  string topwdir = topwdirs;
  if ( !update_mode ) {
    if (topwdirs=="") 
      topwdir = "NONE";
    query = string("INSERT INTO SCHEDULER VALUES (")
      + "\'" + name + "\',"    // name
      + "\'\',"                // default
      + "\'\',"                // top working dir
      + "\'\',"                // submit script
      + "\'\',"                // kill script
      + "\'\')";               // query script
    if (accept(query)!=0) {
      return return_val;
    }
  }
  query = string("UPDATE SCHEDULER SET ") + "DEF = \'"+ def +"\',";
  if ( topwdir.size()>0 ) {
    query += "TOP_WORK_DIR = \'"+ topwdir +"\',";
  }
  if ( submit_file != "NULL" ) {
    unsigned int blen;
    sys->fileSize(submit_file, &blen);
    query += "SUBMIT_BIN = \'"+ escape_file(submit_file, blen) +"\',";
  }
  if ( kill_file != "NULL" ) {
    unsigned int blen;
    sys->fileSize(kill_file, &blen);
    query += "KILL_BIN = \'"+ escape_file(kill_file, blen) +"\',";
  }
  if ( query_file != "NULL" ) {
    unsigned int blen;
    sys->fileSize(query_file, &blen);
    query += "QUERY_BIN = \'"+ escape_file(query_file, blen) +"\',";
  }
  query = query.substr(0,query.size()-1) + " WHERE NAME = \'" + name    + "\'";
  
  if (accept(query)!=0) {
    return return_val;
  }
  return_val = 0;
  clearResults(current_results);
  return return_val;

}

// 0 succesful
// -1 error
int BossDatabase::deleteScheduler (string name) {
  int return_val = -1;

  // check if exist
  string query = "SELECT * FROM SCHEDULER WHERE NAME =\'" + name + "\'";
  // DEBUG
  // cout << query << endl;
  // END DEBUG
  MYSQL_RES* current_results = getResults(query);
  if (current_results==0) {
    return return_val;
  }

  if ( mysql_num_rows(current_results) >=1 ) {
    MYSQL_ROW row = mysql_fetch_row(current_results);
    if        ( string(row[1]) == "Y") {
      cerr << "deleteScheduler: Cannot delete the default scheduler!" << endl;
      cerr << "                 Register another default scheduler first!" << endl;
      return return_val;
    }
    string query = "DELETE FROM SCHEDULER WHERE NAME =\'" + name + "\'";
    if (accept(query)!=0) {
      return return_val;
    }
    
    if ( affectedRows() <= 0 ) { // the job is not found 
      return return_val;
    } 
    return_val = 0;
  }
  return return_val;
}

// 0 succesfull
// -1 error
int BossDatabase::registerJob (string name, string schema, string comment,
			       string pre_file, string run_file, string post_file) {

  int return_val = -1;

  // check if job type is already registered
  int update_mode=0;
  string query = "SELECT * FROM JOBTYPE WHERE NAME =\'" + name + "\'";;
  MYSQL_RES* current_results = getResults(query);
  if (current_results==0) {
    return -1;
  }
  if ( mysql_num_rows(current_results) >=1 ) {
    update_mode=1;
  }

  BossOperatingSystem* sys=BossOperatingSystem::instance();

  // set schema and create the table if needed
  string sch = "";
  if ( schema != "NULL" ) {
    if ( !sys->fileExist(schema) ) {
      cerr << "Schema file doesn't exist" << endl;
      return return_val;
    }
    ifstream fsc(schema.c_str());
    BossJobData dat(name, fsc);
    fsc.close();
    sch = dat.getSchema();
    if ( update_mode ) {
      // drop the old table
      query = "DROP TABLE " + name;
      if (accept(query)!=0) {
	return return_val;
      }
    }
    // create the new table
    query = "CREATE TABLE " + name + " (ID INT PRIMARY KEY AUTO_INCREMENT," 
      + "JOBID INT,";
    // iterate on types
    for (BossJobData::const_iterator i = dat.begin(); i != dat.end(); i++)
      query += dat[i].name() + " " + DSLType2SQLType(dat[i].type()) + ",";
    query.erase(query.length()-1, 1);
    query += ")";
    if (table_type_.size()>0)
      query += string(" TYPE=") + table_type_;
    if (accept(query)!=0) {
      return return_val;
    }
    
    // add UPDATE privileges to jobmonitor on jobtype columns that differ from
    // ID and JOBID
    // access allowed from localhost
    query = "GRANT UPDATE(";
    // iterate on columns name
    for (BossJobData::const_iterator i = dat.begin(); i != dat.end(); i++)
      query += dat[i].name() + ",";
    query.erase(query.length()-1, 1);
    query += ") ON " + string(db_) + "." + name + string(" TO ") + 
       "'" + string(guest_user_) + "'" + string("@localhost");
    // DEBUG
    // cout << query << endl;
    // END DEBUG
    if (accept(query)!=0) {
      return return_val;
    }
    
    // access allowed from the boss host 
    query = "GRANT UPDATE(";
    // iterate on columns name
    for (BossJobData::const_iterator i = dat.begin(); i != dat.end(); i++)
      query += dat[i].name() + ",";
    query.erase(query.length()-1, 1);
    query += ") ON " + string(db_) + "." + name + string(" TO ") + "'" +
       string(guest_user_) + "'" + string("@") + "'" + string(hostname_) + "'";
    // DEBUG
    // cout << query << endl;
    // END DEBUG
    if (accept(query)!=0) {
      return return_val;
    }
    
    // access allowed from the domain of boss host if any
    if ( domain_ != 0 ) {
      query = "GRANT UPDATE(";
      // iterate on columns name
      for (BossJobData::const_iterator i = dat.begin(); i != dat.end(); i++)
	query += dat[i].name() + ",";
      query.erase(query.length()-1, 1);
      query += ") ON " + string(db_) + "." + name + string(" TO ") + "'" +
        string(guest_user_) + "'" + string("@'%.") + string(domain_) + "'";
      // DEBUG
      // cout << query << endl;
      // END DEBUG
      if (accept(query)!=0) {
	return return_val;
      }
    }
  }
  // Now update all the entries in the JOBTYPE table
  if ( !update_mode ) {
    query = string("INSERT INTO JOBTYPE VALUES (")
      + "\'" + name + "\'," // name
      + "\'\',"             // schema
      + "\'\',"             // comment
      + "\'\',"             // pre
      + "\'\',"             // runtime
      + "\'\')";            // post
    if (accept(query)!=0) {
      return return_val;
    }
  }
  string val = "";
  if ( !sch.empty() ) {
    val += "SCHEMA = \'" + sch + "\',";
  }
  if ( !comment.empty() ) {
    val += "COMMENT = \'" + comment + "\',";
  }
  if        ( pre_file == "SKIP" ) {
    val += "PRE_BIN = \'\',";
  } else if ( pre_file != "NULL" ) {
    unsigned int blen;
    sys->fileSize(pre_file, &blen);
    val += "PRE_BIN = \'"+ escape_file(pre_file, blen) +"\',";
  }
  if        ( run_file == "SKIP" ) {
    val += "RUN_BIN = \'\',";
  } else if ( run_file != "NULL" ) {
    unsigned int blen;
    sys->fileSize(run_file, &blen);
    val += "RUN_BIN = \'"+ escape_file(run_file, blen) +"\',";
  }
  if        ( post_file == "SKIP" ) {
    val += "POST_BIN = \'\',";
  } else if ( post_file != "NULL" ) {
    unsigned int blen;
    sys->fileSize(post_file, &blen);
    val += "POST_BIN = \'"+ escape_file(post_file, blen) +"\',";
  } 
  if ( !val.empty() ) {
    query = string("UPDATE JOBTYPE SET ") + val.substr(0,val.size()-1) 
      + " WHERE NAME =\'" + name + "\'";
    // cout << query << endl;
    if (accept(query)!=0) {
      return return_val;
    }
  }
  return_val = 0;
  return return_val;
}

// 0 succesful
// -1 error
int BossDatabase::deleteJobType (string name) {
  int return_val = -1;

  // check if exist
  string query = "SELECT * FROM JOBTYPE WHERE NAME =\'" + name + "\'";
  // DEBUG
  // cout << query << endl;
  // END DEBUG
  MYSQL_RES* current_results = getResults(query);
  if (current_results==0) {
    return return_val;
  }

  if ( mysql_num_rows(current_results) >=1 ) {
    // drop the table
    string query = "DROP TABLE " + name;
    if (accept(query)!=0) {
      return return_val;
    }

    // delete the entry in the JOBTYPE table
    query = "DELETE FROM JOBTYPE WHERE NAME =\'" + name + "\'";
    if (accept(query)!=0) {
      return return_val;
    }
    
    if ( affectedRows() <= 0 ) { // the jobType is not found 
      return return_val;
    } 
    return_val = 0;
  }

  return return_val;
}

// return empty string if not existing
string BossDatabase::findSchema(string name) {

  string return_val = "";
  string query = string("SHOW FIELDS FROM ") + name;
  MYSQL_RES* current_results = getResults(query);
  if (current_results==0) {
    cerr << "Table not known." << endl;
    return return_val;
  }
  if ( mysql_num_rows(current_results) <=0 ) {
    cerr << "Wrong table format" << endl;
    clearResults(current_results);
    return return_val;
  } 

  MYSQL_ROW row;
  while ( (row = mysql_fetch_row(current_results)) ) {
    string var=row[0];
    if ( name!="JOB" && (var=="ID"||var=="JOBID") )
      continue;
    string type=row[1];
    type = type.substr(0,type.find_first_of('(',0));
    if ( type == "varchar" )type = "string";
    return_val+=var+":"+type+",";
  }

  clearResults(current_results);

  return return_val.substr(0,return_val.size()-1);
  
//    string return_val = "";

//    string query = string("SELECT SCHEMA FROM JOBTYPE ") +
//                 + "WHERE NAME =\'" + name + "\'";
//    MYSQL_RES* current_results = getResults(query);
//    if (current_results==0) {
//      return return_val;
//    }

//    if ( mysql_num_rows(current_results) <=0 ) {
//      cout << "DB inconsistency !! No schema avaible" << endl;
//      clearResults(current_results);
//      return return_val;
//    } 

//    MYSQL_ROW row = mysql_fetch_row(current_results);
//    return_val = string(row[0]);

//    clearResults(current_results);
//    return return_val;
}


string BossDatabase::DSLType2SQLType(string type) {

  if ( type == "string" ) 
    return string("BLOB");
  if ( type == "int" )
    return string("INT");
  return "";
}

string BossDatabase::DSLData2SQLData(string data, string type) {

  if ( type == "string" ) 
    return string("\'")+data+"\'";
  if ( type == "int" )
    return data;
  return "";
}

int BossDatabase::SQLquery(string query, ostream& result) {

  int return_val = -1;

  MYSQL_RES* current_results;
  if ( tryQuery(current_connection_, query) ) {
    fatal("ExecQuery (1) ",800);
    return return_val;
  }
  current_results = mysql_store_result(current_connection_);
  if ( !current_results ) {
    // query returned nothing, is it an UPDATE ?
    if ( mysql_field_count(current_connection_) == 0 ) {
      cout << "Affected rows " << mysql_affected_rows(current_connection_) 
	   << endl;
      return 0;
    } else {
      fatal("ExecQuery (2) ",800);
      return return_val;
    }
  }

  // get the fields
  unsigned int num_fields = mysql_num_fields(current_results);
  MYSQL_FIELD* fields = mysql_fetch_fields(current_results);

  // compute the max length of fields 
  unsigned int* fields_max_l = new unsigned int[num_fields];
  unsigned int l;
  for (unsigned int i=0; i < num_fields; i++) {
    l = strlen(fields[i].name);
    if ( fields[i].max_length >= l )
      fields_max_l[i] = fields[i].max_length;
    else 
      fields_max_l[i] = l;
  }

  // return the number of fields and the list of length of each field 
  result << num_fields;
  for (unsigned int i=0; i < num_fields; i++) {
    result << "," << fields_max_l[i]+2;
  }
  result << endl;

  // header
  for (unsigned int i=0; i < num_fields; i++) {
    result << wj(string(" ")+fields[i].name,fields_max_l[i]+2, "left");
  }
  result << endl;

  // tuples
  MYSQL_ROW row;
  string v;
  while ( (row = mysql_fetch_row(current_results)) ) {
    for (unsigned int i=0; i < num_fields; i++) {
      if ( row[i] == NULL )
	v = "NULL";
      else
	v = row[i];
      result << wj(string(" ") + v,fields_max_l[i]+2, "left");
    }
    result << endl;
  }  
  clearResults(current_results);

  return 0;
}

vector<string> BossDatabase::schedulers() {

  vector<string> return_val;

  //  if (!connect())return return_val;

  string query = "SELECT NAME FROM SCHEDULER";
  MYSQL_RES* current_results = getResults(query);
  if (current_results==0) {
    return return_val;
  }
  if ( mysql_num_rows(current_results) <=0 ) {
    cout << "DB inconsistency !! No scheduler avaible" << endl;
    clearResults(current_results);
    return return_val;
  } 

  MYSQL_ROW row;
  while ( (row = mysql_fetch_row(current_results)) ) {
    return_val.push_back(row[0]);
  }

  clearResults(current_results);
  return return_val;
}

vector<string> BossDatabase::jobTypes() {

  vector<string> return_val;

  string query = "SELECT NAME,COMMENT FROM JOBTYPE";
  MYSQL_RES* current_results = getResults(query);
  if (current_results==0) {
    return return_val;
  }
  if ( mysql_num_rows(current_results) <=0 ) {
    cout << "DB inconsistency !! No jobtype avaible" << endl;
    clearResults(current_results);
    return return_val;
  } 

  MYSQL_ROW row;
  while ( (row = mysql_fetch_row(current_results)) ) {
    return_val.push_back(string(row[0])+"\t"+row[1]);
  }

  clearResults(current_results);
  return return_val;

}

vector<BossJob*> BossDatabase::jobs(char option, string type, string user, string format) {
  vector<BossJob*> return_val;
  string what = "";
  string options = "";
  if ( type != "" ) {
    if ( ! existJobType(type) ) { 
      cerr << "JobType " << type << " doesn't exist!" << endl;
      type = "";
    }
  }
  if ( format == "specific" && type != "" ) {
    what = "* FROM JOB, " + type;
    options = "JOB.ID = " + type + ".JOBID ";
  } else {
    what = "* FROM JOB";
  }
  if ( user != "" ) {
    if (options != "" )
      options += " AND";
    options += " S_USR = \'" + user + "\' ";
  }
  // optimize the query
  if ( option == 'r' ) {
    if (options != "" )
      options += " AND";
    options += " T_START <> 0 AND T_STOP = 0";
  }
  if ( option == 's' ) {
    if (options != "" )
      options += " AND";
    options += " T_STOP = 0";
  }
  string query = "SELECT "+what;
  if (options != "" ) query +=" WHERE "+options;
  query +=" ORDER BY JOB.ID";

  // DEBUG
  // cout << query << endl;
  // END DEBUG

  MYSQL_RES* current_results = getResults(query);
  if (current_results==0) {
    cerr << "No connection avaible !!" << endl;
    return return_val;
  }

  if ( mysql_num_rows(current_results) <=0 ) {
    cout << "No jobs found !!" << endl;
    clearResults(current_results);
    return return_val;
  } 

  string schema = "";
  if ( type != "" && type != "stdjob" ) {
    schema = findSchema(type);
    // DEBUG
    // cout << schema << endl;
    // END DEBUG
  }

  MYSQL_ROW row;
  // DEBUG
  // cout << " Scanning " << mysql_num_rows(current_results) << " records" << endl;
  // END DEBUG
  // get the fields structure
  unsigned int num_fields = mysql_num_fields(current_results);
  MYSQL_FIELD* fields = mysql_fetch_fields(current_results);

  // DEBUG
  // for (unsigned int i=0; i!= num_fields; i++) {
  //  string n = fields[i].name;
  // cout << i << ":" << n << endl;
  // }
  // END DEBUG

  while ( (row = mysql_fetch_row(current_results)) ) {
    
    BossJob* jobH = new BossJob();
    std::stringstream sch;
    sch << schema;
    jobH->setSchema(type,sch);

    int id = atoi(row[0]);
    if (id <= 0)
      cerr << row[0] << endl;
    jobH->setId(id);
    for (unsigned int i=1; i!= num_fields; i++) {
      string n = fields[i].name;
      string v = string(row[i]);
      string tab = type;
      if ( i<21 ) // JOB has 20 columns !!
	tab = "JOB";
      if( string(fields[i].name)!="ID" && string(fields[i].name)!="JOBID" ) {
	jobH->setData(BossUpdateElement(id,tab,fields[i].name,string(row[i])));
      }
    }
    if(!jobH) continue;
    if (type == "" || jobH->isOfType(type)) {
      return_val.push_back(jobH);
    } else {
      delete jobH;
    }
  }

  clearResults(current_results);
  // DEBUG
  // cout << " returning job vector, size is " << return_val.size() << endl;
  // END DEBUG
  return return_val;

}

// fit the string data in width chars justifing it left or right
string BossDatabase::wj(string data, unsigned int width, string justify) {

  string str;

  unsigned int len = data.length(); 

  if ( len < width ) {
    unsigned int w = width - len;
    if ( justify == "left" ) {
      str = data + string(w,' ');
    }else if ( justify == "right" )
      str = string(w,' ') + data;
    else 
      str = data + string(w,' ');
  } else {
    if ( justify == "right" ) {
       str = data.substr(len-width,width);
    }else if ( justify == "left" )
      str = data.substr(0,width);
    else 
      str = data.substr(0,width);
  }
  return str;
}


string BossDatabase::escape_file(string filename, unsigned int len) {
  
  ifstream In(filename.c_str(),ios::in | ios::binary); 
  char  *read_buffer = new char[len];
  char  *escaped_buffer = new char[len*2];
  In.read(read_buffer,len);
  mysql_real_escape_string(current_connection_,escaped_buffer,read_buffer,len);
  return string(escaped_buffer);

}

int 
BossDatabase::existColumn(string name, string table) {
  int ret_val = 0;
  if(table == "JOB" || (existJobType(table) && table!="stdjob" )) {
    string schema = findSchema(table);
    std::stringstream sch;
    sch << schema;
    BossJobData dat(table, sch);
    if(dat.find(name)!=dat.end())
      ret_val = 1;
  } else {
    cout << "Job type " << table << " does not exist! " << endl;
  }
  return ret_val;
}
