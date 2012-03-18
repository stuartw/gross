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

#include "OperatingSystem.h"
#include "BossJob.h"
#include "BossUpdateElement.h"

#include "SirDBBackend.h"

using namespace std;

BossDatabase::BossDatabase(string mode) : 
  mode_(mode), last_err_time_(0) {
  
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
  
  backend = SirDBBackend::create(config->boss_config_file(),mode);
}


BossDatabase::~BossDatabase() {
  delete backend;
}

// 1         the job exist
// 0         the job doesn't exist or there was an error
int BossDatabase::existJob(int id) {

  int return_val = 0;
  
  string query = string("SELECT ID FROM JOB WHERE ID =") + OSUtils::convert2string(id);
  if(backend->rowcount_query(query) > 0)
    return_val = 1;

  return return_val;
}

// 1         the job type exist
// 0         the job type doesn't exist or there was an error
int BossDatabase::existJobType(string job_type) {

  int return_val = 0;

  string query = "SELECT name FROM JOBTYPE WHERE NAME =\'" + job_type + "\'";
  if(backend->rowcount_query(query) > 0)
    return_val = 1;

  return return_val;
}

// 1        the job exist
// 0        the job doesn't exist or there was an error
int BossDatabase::existSchType(string sch) {

  int return_val = 0;

  string query = "SELECT * FROM SCHEDULER WHERE NAME =\'" + sch + "\'";;
  if(backend->rowcount_query(query) > 0)
    return_val = 1;

  return return_val;
}

// return "NULL" if there was an error default scheduler
// -1       there was an error
string BossDatabase::getDefaultSch() {

  string sch = "NULL";

  string query = "SELECT name FROM SCHEDULER WHERE def =\'Y\'";
  SirDBResultSet results = backend->fetch_query(query);
  if(results.nrows() != 1)
    cout << "DB inconsistency !! there is no default scheduler !!" << endl;
  else
    sch = string(results[0][0]);

  return sch;
}

  

string BossDatabase::getWorkDir(string name) {

  string return_val = "";

  string query = "SELECT TOP_WORK_DIR FROM SCHEDULER WHERE NAME =\'" + name + "\'";;
  SirDBResultSet results = backend->fetch_query(query);
  if(results.nrows() != 1)
    cout << "DB inconsistency !! Top working dir not set for scheduler !!" << endl;
  else
    return_val = string(results[0][0]);

  return return_val;

}

// 1 job inserted
// 0 job not inserted
int BossDatabase::insertJob(BossJob* jobH) {

  // add a new job and return the unique job handle
  int return_val=0;

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
  int jobID = backend->insert_query(query);
  if(jobID < 0)
    return return_val;
  else if(jobID == 0)
    jobID = -1;

  jobH->setId(jobID);

  // create job specific entries
  vector<string> types = jobH->getJobTypes();
  for(vector<string>::const_iterator ti=types.begin(); ti<types.end(); ti++) {
    if ( *ti != "stdjob" ) {
      string names = "ID,JOBID";
      string values = string("NULL,") + OSUtils::convert2string(jobID);
      // At the time of insertion these are empty comment out
//       BossJob::const_iterator i;
//       for (i=jobH->beginSpecific(); i!= jobH->endSpecific(); i++ ) {
// 	string n = jobH->specificData(i).name();
// 	if(jobH->existColumn((*ti),n)) {
// 	  string v = jobH->specificData(i).value();
// 	  string t = jobH->specificData(i).type();
// 	  string delimiter = "\'";
// 	  //      if (t == "int" || t == "INT" ) delimiter = "";
// 	  names += "," + n;
// 	  values += "," + delimiter + v + delimiter;
// 	}
//       }
      string query = "INSERT INTO "+(*ti)+"(" + names +
	") VALUES ("       + values + ")";
      if(backend->query(query) != 0) {
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

  string query = string("DELETE FROM JOB WHERE ID = ") + OSUtils::convert2string(jobID);
  if(backend->rowcount_query(query) <= 0)
    return return_val;
        
  return 0;
} 

// 0 successful
// <0 error
int BossDatabase::deleteSpecificJob(string type, int jobID) {

  int return_val = -1;

  string query = string("DELETE FROM ") + type + " WHERE JOBID = " + OSUtils::convert2string(jobID);
 
  if(backend->rowcount_query(query) <= 0)
    return return_val;

  return 0;
}

// 0 successful
// <0 error
int BossDatabase::getGeneralJobData(BossJob* jobH) {

  int return_val = -1;

  if (!jobH)return return_val;
  
  int id = jobH->getId();
  if (id <= 0) return return_val;
  string strid = OSUtils::convert2string(id);
  string query = string("SELECT * FROM JOB WHERE ID = ") + strid;

  SirDBResultSet results = backend->fetch_query(query);
  if(!results || results.nrows() == 0)
    return return_val;

  SirDBResultHeaders headers = results.headers();
  SirDBResultRow row = results[0];
  jobH->setId(atoi(row[0]));
  for(unsigned int i = 1; i < results.nfields(); i++)
    jobH->setData(BossUpdateElement(id, "JOB", headers[i], row[i]));

  return 0;

}


// 0 successful
// <0 error
int BossDatabase::getSpecificJobData(BossJob* jobH) {

  int return_val = -1;

  if (!jobH)return return_val;

  int id = jobH->getId();
  if (id <= 0) return return_val;
  string strid = OSUtils::convert2string(id);
  vector<string> types = jobH->getJobTypes();
  for(vector<string>::const_iterator ti=types.begin(); ti<types.end(); ti++) {
    if ( *ti != "stdjob" ) {
      if ( existJobType(*ti) ) {
	string query = string("SELECT * FROM "+(*ti)+" WHERE JOBID = ") + strid;
        SirDBResultSet results = backend->fetch_query(query);
        if(!results || results.nrows() == 0)
          return return_val;

        SirDBResultHeaders headers = results.headers();
        SirDBResultRow row = results[0];
        for(unsigned int i = 0; i < results.nfields(); i++) {
          string n = headers[i];
          string v = row[i];
	  //      cout << "***" << n << " " << v << endl;
	  if(n != "ID" && n != "JOBID")
	    jobH->setData(BossUpdateElement(id ,*ti, n, v));
        }

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
  SirDBResultSet results = backend->fetch_query(query);
  if(!results || results.nrows() == 0)
    return -1;

  blob = results[0][0];
  if(blob == "NULL" || blob == "")
    return 1;

  ofstream f(fname.c_str());
  f << blob; 
  f.close();
  // change privileges
  OSUtils::fileChmod("744",fname);
  
  return 0;
}

// 0 successful
// <0 error
// >0 file not stored
int BossDatabase::getKill(string name, string fname) {

  string query;
  string blob = "";

  query = "SELECT KILL_BIN FROM SCHEDULER WHERE NAME = \"" + name + "\""; 
  SirDBResultSet results = backend->fetch_query(query);
  if(!results || results.nrows() == 0)
    return -1;

  blob = results[0][0];
  if(blob == "NULL" || blob == "")
    return 1;

  ofstream f(fname.c_str());
  f << blob; 
  f.close();
  // change privileges
  OSUtils::fileChmod("744",fname);
  
  return 0;
}

// 0 successful
// <0 error
// >0 file not stored
int BossDatabase::getQuery(string name, string fname) {

  string query;
  string blob = "";

  query = "SELECT QUERY_BIN FROM SCHEDULER WHERE NAME = \"" + name + "\""; 
  SirDBResultSet results = backend->fetch_query(query);
  if(!results || results.nrows() == 0)
    return -1;

  blob = results[0][0];
  if(blob == "NULL" || blob == "")
    return 1;

  ofstream f(fname.c_str());
  f << blob; 
  f.close();
  // change privileges
  OSUtils::fileChmod("744",fname);
  
  return 0;
}

// 0 successful
// <0 error
// >0 file not stored
string BossDatabase::getCopy(string name) {

  string return_val = "";

  string query = "SELECT COPY_COMMAND FROM SCHEDULER WHERE NAME =\'" + name + "\'";;
  SirDBResultSet results = backend->fetch_query(query);
  if(results.nrows() != 1)
    cout << "DB inconsistency !! Copy command not set for scheduler !!" << endl;
  else
    return_val = string(results[0][0]);

  return return_val;

}

// 0 successful
// <0 error
// >0 file not stored
int BossDatabase::getPreProcess(string type, string fname) {

  string query;
  string blob = "";

  query = "SELECT PRE_BIN FROM JOBTYPE WHERE NAME = \"" + type + "\""; 
  SirDBResultSet results = backend->fetch_query(query);
  if(!results || results.nrows() == 0)
    return -1;

  blob = results[0][0];
  if(blob == "NULL" || blob == "")
    return 1;
  
  ofstream f(fname.c_str());
  f << blob; 
  f.close();
  // change privileges
  OSUtils::fileChmod("744",fname);

  return 0;
}

int BossDatabase::getRuntimeProcess(string type, string fname) {

  string query;
  string blob = "";

  query = "SELECT RUN_BIN FROM JOBTYPE WHERE NAME = \"" + type + "\""; 
  SirDBResultSet results = backend->fetch_query(query);
  if(!results || results.nrows() == 0)
    return -1;

  blob = results[0][0];
  if(blob == "NULL" || blob == "")
    return 1;

  ofstream f(fname.c_str());
  f << blob; 
  f.close();
  // change privileges
  OSUtils::fileChmod("744",fname);
  
  return 0;
}

int BossDatabase::getPostProcess(string type, string fname) {

  string query;
  string blob = "";

  query = "SELECT POST_BIN FROM JOBTYPE WHERE NAME = \"" + type + "\""; 
  SirDBResultSet results = backend->fetch_query(query);
  if(!results || results.nrows() == 0)
    return -1;

  blob = results[0][0];
  if(blob == "NULL" || blob == "")
    return 1;

  ofstream f(fname.c_str());
  f << blob; 
  f.close();
  // change privileges
  OSUtils::fileChmod("744",fname);
 
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
      } else {
	cerr << "Job type " << (*ti) << " does not exisit! " << endl;
      }
    }
  }
  if(getSpecificJobData(jobH)!=0)return 0;

  // Set the default update mode to be the database for future operations
  jobH->setUpdator(this);

  return jobH;
}

// 1 job updated
// 0 job not updated
int BossDatabase::updateJobParameter(int id, string tab, string key, string val) {
  int return_val=0;

  if(existColumn(key,tab)) {
    string idn = "JOBID";
    if (tab == "JOB") idn="ID";
    string query = "UPDATE "+tab+" SET "+key+"=\'"+val+
      "\' WHERE "+idn+"="+ OSUtils::convert2string(id);
    if(backend->query(query) == 0)
      return_val=1;
  }
  return return_val;
}

// 0 succesful
// -1 error
int BossDatabase::registerScheduler (string name, string defs, string topwdirs,
				     string submit_file, string kill_file, 
				     string query_file, string copycomm) {

  int return_val = -1;

  // check if exist
  int update_mode=0;
  int was_default=0;
  string query = "SELECT * FROM SCHEDULER WHERE NAME =\'" + name + "\'";;
  SirDBResultSet results = backend->fetch_query(query);
  if(!results)
    return return_val;
  if(results.nrows() >= 1) {
    update_mode = 1;
    if(string(results[0][1]) == "Y")
      was_default = 1;
  }

  // Check consistency of "default" attribute
  string def = (defs=="TRUE") ? "Y" : "N";
  if         (!was_default && def=="Y") { 
    query = string("UPDATE SCHEDULER SET ")
      + "DEF = \'N\'"
      + " WHERE DEF = \'Y\'";
    if(backend->query(query) != 0) {
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
      + "\'\',"                // query script
      + "\'\')";               // copy script
    if(backend->query(query) != 0) {
      return return_val;
    }
  }
  query = string("UPDATE SCHEDULER SET ") + "DEF = \'"+ def +"\',";
  if ( topwdir.size()>0 ) {
    query += "TOP_WORK_DIR = \'"+ topwdir +"\',";
  }
  if ( submit_file != "NULL" ) {
    unsigned int blen = 0;
    OSUtils::fileSize(submit_file, &blen);
    query += "SUBMIT_BIN = \'"+ escape_file(submit_file, blen) +"\',";
  }
  if ( kill_file != "NULL" ) {
    unsigned int blen = 0;
    OSUtils::fileSize(kill_file, &blen);
    query += "KILL_BIN = \'"+ escape_file(kill_file, blen) +"\',";
  }
  if ( query_file != "NULL" ) {
    unsigned int blen = 0;
    OSUtils::fileSize(query_file, &blen);
    query += "QUERY_BIN = \'"+ escape_file(query_file, blen) +"\',";
  }
  if ( copycomm.size()>0 && copycomm != "NULL") {
    query += "COPY_COMMAND = \'"+ copycomm +"\',";
  }
  query = query.substr(0,query.size()-1) + " WHERE NAME = \'" + name    + "\'";
  
  if(backend->query(query) != 0) {
    return return_val;
  }
  return_val = 0;
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
  SirDBResultSet results = backend->fetch_query(query);
  if(!results)
    return return_val;
  if(results.nrows() >= 1) {
    if(string(results[0][1]) == "Y") {
      cerr << "deleteScheduler: Cannot delete the default scheduler!" << endl;
      cerr << "                 Register another default scheduler first!" << endl;
      return return_val;
    }
    string query = "DELETE FROM SCHEDULER WHERE NAME =\'" + name + "\'";
    if(backend->rowcount_query(query) <= 0)
      return return_val;
    else
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
  int rowcount = backend->rowcount_query(query);
  if(rowcount < 0)
    return -1;
  else if(rowcount >= 1)
    update_mode = 1;

  // set schema and create the table if needed
  string sch = "";
  if ( schema != "NULL" ) {
    if ( !OSUtils::fileExist(schema) ) {
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
      if(backend->query(query) != 0) {
	return return_val;
      }
    }
    // create the new table
    query = "CREATE TABLE " + name + " (ID INT PRIMARY KEY AUTO_INCREMENT," 
      + "JOBID INT REFERENCES JOB(ID),";
    // iterate on types
    for (BossJobData::const_iterator i = dat.begin(); i != dat.end(); i++)
      query += dat[i].name() + " " + DSLType2SQLType(dat[i].type()) + ",";
    query.erase(query.length()-1, 1);
    query += ")";
    if (table_type_.size()>0)
      query += string(" TYPE=") + table_type_;
    if(backend->query(query) != 0) {
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
    if(backend->query(query) != 0) {
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
    if(backend->query(query) != 0) {
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
      if(backend->query(query) != 0) {
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
    if(backend->query(query) != 0) {
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
    unsigned int blen = 0;
    OSUtils::fileSize(pre_file, &blen);
    val += "PRE_BIN = \'"+ escape_file(pre_file, blen) +"\',";
  }
  if        ( run_file == "SKIP" ) {
    val += "RUN_BIN = \'\',";
  } else if ( run_file != "NULL" ) {
    unsigned int blen = 0;
    OSUtils::fileSize(run_file, &blen);
    val += "RUN_BIN = \'"+ escape_file(run_file, blen) +"\',";
  }
  if        ( post_file == "SKIP" ) {
    val += "POST_BIN = \'\',";
  } else if ( post_file != "NULL" ) {
    unsigned int blen = 0;
    OSUtils::fileSize(post_file, &blen);
    val += "POST_BIN = \'"+ escape_file(post_file, blen) +"\',";
  } 
  if ( !val.empty() ) {
    query = string("UPDATE JOBTYPE SET ") + val.substr(0,val.size()-1) 
      + " WHERE NAME =\'" + name + "\'";
    // cout << query << endl;
    if (backend->query(query) != 0) {
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
  int rowcount = backend->rowcount_query(query);
  if(rowcount < 0)
    return return_val;
  else if(rowcount >= 1) {
    // drop the table
    string query = "DROP TABLE " + name;
    if(backend->query(query) != 0)
      return return_val;

    // delete the entry in the JOBTYPE table
    query = "DELETE FROM JOBTYPE WHERE NAME =\'" + name + "\'";
    if(backend->rowcount_query(query) <= 0) // the jobType is not found
      return return_val;

    return_val = 0;
  }

  return return_val;
}

// return empty string if not existing
string BossDatabase::findSchema(string name) {

  string return_val = "";
  string query = string("SHOW FIELDS FROM ") + name;
  SirDBResultSet results = backend->fetch_query(query);
  if(!results) {
    cerr << "Table not known." << endl;
    return return_val;
  }
  else if(results.nrows() == 0) {
    cerr << "Wrong table format" << endl;
    return return_val;
  } 

  SirDBResultRow row;
  size_t nrows = results.nrows();
  for(unsigned int i = 0; i < nrows; i++) {
    row = results[i];
    string var = row[0];
    if((var == "ID" || var == "JOBID") && name != "JOB")
      continue;
    string type = row[1];
    type = type.substr(0, type.find_first_of('('));
    if(type == "varchar")
      type = "string";
    return_val += var + ":" + type + ",";
  }

  return return_val.substr(0, return_val.size()-1);
  
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

int BossDatabase::SQLquery(string query, ostream& out) {

  int return_val = -1;

  // Get the results of the query
  SirDBResultSet results = backend->fetch_query(query);
  if(!results) {
    cerr << "SQLQuery: " << backend->errormsg() << endl;
    return return_val;
  }
  if(results.nrows() == 0) {
    cout << "No results" << endl;
    return return_val;
  }
  SirDBResultHeaders headers = results.headers();
  unsigned int nfields = results.nfields();
  unsigned int nrows = results.nrows();

  // Calculate field widths
  vector<unsigned int> fields_max_l(nfields);
  for(unsigned int j = 0; j < nfields; j++) {
    //std::cout << "SQLquery getting header " << j << "/" << nfields << std::endl;
    fields_max_l[j] = strlen(headers[j]);
  }

  SirDBResultRow row;
  for(unsigned int i = 0; i < nrows; i++) {
    //std::cout << "SQLquery getting row " << i << "/" << nrows << std::endl;
    row = results[i];
    unsigned int l = 0;
    for(unsigned int j = 0; j < nfields; j++) {
      //std::cout << "   ---> SQLquery getting field " << j << "/" << nfields << std::endl;
      l = strlen(row[j]);
      if(l > fields_max_l[j])
        fields_max_l[j] = l;
    }
  }

  // Write the number of fields and a list of the lengths of each field 
  out << nfields;
  for(unsigned int j = 0; j < nfields; j++)
    out << "," << fields_max_l[j] + 2;
  out << endl;

  // Write the headers
  for(unsigned int j = 0; j < nfields; j++)
    out << wj(headers[j], fields_max_l[j] + 2, "left");
  out << endl;

  // Write the rows
  for(unsigned int i = 0; i < nrows; i++) {
    SirDBResultRow row = results[i];
    for(unsigned int j = 0; j < nfields; j++) {
      if(row[j] == NULL)
        out << wj("NULL", fields_max_l[j] + 2, "left");
      else
        out << wj(row[j], fields_max_l[j] + 2, "left");
    }
    out << endl;
  }  

  return 0;
}

vector<string> BossDatabase::schedulers() {

  vector<string> return_val;

  //  if (!connect())return return_val;

  string query = "SELECT NAME FROM SCHEDULER";
  SirDBResultSet results = backend->fetch_query(query);
  if(!results)
    ;
  else if(results.nrows() == 0)
    cout << "DB inconsistency !! No scheduler avaible" << endl;
  else {
    for(unsigned int i = 0; i < results.nrows(); i++)
      return_val.push_back(results[i][0]);
  }
  return return_val;
}

vector<string> BossDatabase::jobTypes() {

  vector<string> return_val;

  string query = "SELECT NAME,COMMENT FROM JOBTYPE";
  SirDBResultSet results = backend->fetch_query(query);
  if(!results)
    ;
  else if(results.nrows() == 0)
    cout << "DB inconsistency !! No jobtype avaible" << endl;
  else {
    for(unsigned int i = 0; i < results.nrows(); i++) {
      SirDBResultRow row = results[i];
      return_val.push_back(string(row[0]) + '\t' + row[1]);
    }
  }
  return return_val;

}

vector<BossJob*> BossDatabase::jobs(BossJobIDRange idr, char option, string type, string user, string format) {
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
  query +=" WHERE JOB.ID>="+idr.sfirst()+
    " AND JOB.ID<="+idr.slast();
  if (options != "" ) query +=" AND "+options;
  query +=" ORDER BY JOB.ID";

  // DEBUG
  // cout << query << endl;
  // END DEBUG

  SirDBResultSet results = backend->fetch_query(query);
  if(!results) {
    cerr << "No connection avaible !!" << endl;
    return return_val;
  }
  else if(results.nrows() == 0) {
    cout << "No jobs found !!" << endl;
    return return_val;
  } 

  string schema = "";
  if ( type != "" && type != "stdjob" ) {
    schema = findSchema(type);
    // DEBUG
    // cout << schema << endl;
    // END DEBUG
  }

  SirDBResultHeaders headers = results.headers();
  for(unsigned int i = 0; i < results.nrows(); i++) {
    SirDBResultRow row = results[i];

    BossJob* jobH = new BossJob();
    std::stringstream sch;
    sch << schema;
    jobH->setSchema(type,sch);

    int id = atoi(row[0]);
    if (id <= 0)
      cerr << row[0] << endl;
    jobH->setId(id);

    for(unsigned int i = 1; i < headers.nfields(); i++) {
      string n = headers[i];
      string v = string(row[i]);
      string tab = type;
      if(i < 25) // JOB has 24 columns !!  To be modified!!!
	tab = "JOB";
      if(string(headers[i]) != "ID" && string(headers[i]) != "JOBID")
	jobH->setData(BossUpdateElement(id, tab, headers[i], row[i]));
    }
    if(type == "" || jobH->isOfType(type))
      return_val.push_back(jobH);
    else
      delete jobH;
  }
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
  In.read(read_buffer,len);
  return backend->escape(read_buffer, len);

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
