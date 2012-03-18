// /////////////////////////////////////////////////////////////////////
// Program: SirDB
// Version: 1.0
// File:    MySQLBackend.cc
// Authors: Jordan Carlson (Caltech), Claudio Grandi (INFN BO)
// Date:    16/09/2004
// Note:
// /////////////////////////////////////////////////////////////////////

#include <cassert>
#include <iostream>

#include "MySQLBackend.h"

using namespace std;


/***** Convenience functions *****/

static void warn(const std::string& where, const std::string& why) {
  cerr << "[WARNING] " << where << ": " << why << endl;
}


/***** MySQLBackend *****/

MySQLBackend::MySQLBackend(const std::string& config, const std::string& mode) 
  : contact_(config, mode), _mysql_handle(0) {
  connect();
}

MySQLBackend::~MySQLBackend() {
  disconnect();
}

int MySQLBackend::connect() {
  /* We only need one connection */
  if(_mysql_handle)
    return 0;

  /* Initialize the MySQL library */
  _mysql_handle = mysql_init(0);
  if(!_mysql_handle) {
    warn("connect", "could not initialize MySQL");
    return -1;
  }

  /* Connect to the database */
  if(!mysql_real_connect(_mysql_handle, 
			 contact_.db_host(), 
			 contact_.db_user(), 
			 contact_.db_user_pw(), 
			 contact_.db_name(),
			 contact_.db_port(),
			 contact_.db_unix_socket(), 
			 contact_.db_client_flag())) {
    warn("connect", errormsg());
    disconnect();
    return -1;
  }
  return 0;
}

void MySQLBackend::disconnect() {
  if(_mysql_handle)
    mysql_close(_mysql_handle);
  _mysql_handle = 0;
}

int MySQLBackend::query(const std::string& q) {
  // Make sure we are connected
  if(!_mysql_handle) {
    warn("query", errormsg());
    return -1;
  }

  // Execute the query
  if(mysql_query(_mysql_handle, q.c_str()) != 0) {
    warn("query", errormsg());
    return -1;
  }

  return 0;
}

SirDBResultSet MySQLBackend::fetch_query(const std::string& q) {
  if(query(q) != 0)
    return SirDBResultSet(0);

  // Get the results, if there are any
  MYSQL_RES* res = mysql_store_result(_mysql_handle);
  if(res)
    return SirDBResultSet(new MySQL_ResultSetData(res));
  else {
    // If there should have been results, print a warning and continue
    if(mysql_field_count(_mysql_handle) != 0)
      warn("fetch_query", errormsg());
    return SirDBResultSet(0);
  }
}

int MySQLBackend::rowcount_query(const std::string& q) {
  if(query(q) != 0)
    return -1;

  if(mysql_field_count(_mysql_handle) == 0)
    return mysql_affected_rows(_mysql_handle);
  else {
    MYSQL_RES* res = mysql_store_result(_mysql_handle);
    int count = (res == 0) ? -1 : (int)mysql_num_rows(res);
    mysql_free_result(res);
    return count;
  }
}

int MySQLBackend::insert_query(const std::string& q) {
  if(query(q) != 0)
    return -1;
  else
    return mysql_insert_id(_mysql_handle);
}

std::string MySQLBackend::escape(std::string s) {
  size_t len = s.length();
  char buffer[2*len + 1];
  mysql_real_escape_string(_mysql_handle, buffer, s.data(), len);
  return string(buffer);
}

std::string MySQLBackend::escape(const char* s, size_t len) {
  char buffer[2*len + 1];
  mysql_real_escape_string(_mysql_handle, buffer, s, len);
  return string(buffer);
}

std::string MySQLBackend::errormsg() {
  if(!_mysql_handle)
    return "not connected";
  else
    return mysql_error(_mysql_handle);
}


/***** MySQL_ResultSetData *****/

MySQL_ResultSetData::MySQL_ResultSetData(MYSQL_RES* mysql_res) {
  res = mysql_res;
  nrows = mysql_num_rows(res);
  nfields = mysql_num_fields(res);
}

MySQL_ResultSetData::~MySQL_ResultSetData() {
  mysql_free_result(res);
}

const std::vector<std::vector<char*> >* MySQL_ResultSetData::getrows() {
  if(rows.empty() && nrows != 0) {
    rows.resize(nrows);
    for(unsigned int i = 0; i < nrows; i++) {
      std::vector<char*>& row = rows[i];
      row.resize(nfields);
      MYSQL_ROW myrow = mysql_fetch_row(res);
      for(unsigned int j = 0; j < nfields; j++) {
	if ( myrow[j] == NULL )
	  row[j] = "NULL";
	else
	  row[j] = myrow[j];
      }
    }
    assert(mysql_fetch_row(res) == 0); // make sure we got all the rows
  }
  return &rows;
}

const std::vector<char*>* MySQL_ResultSetData::getheaders() {
  if(headers.empty() && nfields != 0) {
    headers.resize(nfields);
    MYSQL_FIELD* myfields = mysql_fetch_fields(res);
    for(unsigned int j = 0; j < nfields; j++)
      headers[j] = myfields[j].name;
  }
  return &headers;
}
