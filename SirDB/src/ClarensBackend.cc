// /////////////////////////////////////////////////////////////////////
// Program: SirDB
// Version: 1.0
// File:    ClarensBackend.cc
// Authors: Jordan Carlson (Caltech), Claudio Grandi (INFN BO)
// Date:    16/09/2004
// Note:
// /////////////////////////////////////////////////////////////////////

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <fstream>
#include <iostream>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/sha.h>

#include "OperatingSystem.h"
#include "ClarensBackend.h"

using namespace std;


/***** Auxiliary functions *****/

static void warn(const string& where, const string& why) {
  cerr << "[WARNING] " << where << ": " << why << endl;
}

#if 0
/* Split a string into an array of substrings, using the given separator. */
static vector<string> split(const string& s, char sep = ' ') {
  vector<string> entries;
  int prev = -1;
  unsigned int colon = 0;

  while(colon != string::npos) {
    colon = s.find(sep, prev+1);
    entries.push_back(s.substr(prev+1, colon-prev-1));
    prev = colon;
  }

  return entries;
}

/* Scan a string for shell-like variable references (e.g. "$HOME") and
   replace them with the value of the corresponding environment variable. */
static string expand_env(const string& s) {
  string expanded;
  string var;

  unsigned int len = s.length();
  for(unsigned int k = 0; k < len; k++) {
    if(k < len-1 && s[k] == '$') {
      var = "";
      ++k;
      if(s[k] == '{')
        ++k;
      while(k < len && (isalnum(s[k]) || s[k] == '_'))
        var += s[k++];
      if(k < len && s[k] != '}')
        --k;
      expanded += OSUtils::getEnv(var);
    }
    else
      expanded += s[k];
  }

  return expanded;
}
#endif

/* Return the name of the first file in the list that exists. */
static string find_file(const vector<string>& paths) {

  for(vector<string>::const_iterator pi = paths.begin(); pi != paths.end(); ++pi) {
    if(OSUtils::fileExist(*pi))
      return *pi;
  }

  return "";
}


/***** ClarensBackend *****/

ClarensBackend::ClarensBackend(const std::string& config, const string& mode) 
  : contact_(config), _mysql_handle(0) {
  
  clarens_url = strdup(contact_->clarens_url());

  vector<string> certpaths;
  vector<string> keypaths;
  vector<string> proxypaths;

  /* Look for the user's certificate and key */
  OSUtils::splitString(OSUtils::expandEnv(contact_.clarens_cert()), ':', &certpaths);
  OSUtils::splitString(OSUtils::expandEnv(contact_.clarens_key()), ':', &keypaths);
  string certloc = find_file(certpaths);
  string keyloc = find_file(keypaths);

  /* If not found, look for a proxy certificate */
  if(certloc == "" || keyloc == "") {
    OSUtils::splitString(OSUtils::expandEnv(contact_.clarens_proxy()), ':', &proxypaths);
    certloc = find_file(proxypaths);
    keyloc = certloc;
  }

  if(certloc == "" || keyloc == "") {
    warn("ClarensBackend", "could not find certificate or key");
    cerr << "  The following locations were checked:";
    for(unsigned int i = 0; i < certpaths.size(); ++i)
      cerr << "\n    " << certpaths[i];
    for(unsigned int i = 0; i < keypaths.size(); ++i)
      cerr << "\n    " << keypaths[i];
    for(unsigned int i = 0; i < proxypaths.size(); ++i)
      cerr << "\n    " << proxypaths[i];
    cerr << "\n  Continuing without authentication..." << endl;
  }

  certfile = strdup(certloc.c_str());
  keyfile = strdup(keyloc.c_str());

  client = clarens_client_new(certfile, keyfile, &env);
  xmlrpc_env_init(&env);
  connect();
}

ClarensBackend::~ClarensBackend() {
  disconnect();

  free(clarens_url);
  free(certfile);
  free(keyfile);

  if(client)
    clarens_client_free(client);

  xmlrpc_env_clean(&env);
}

int ClarensBackend::connect() {
  if(!client)
    return -1;

  /* The Clarens client has problems with decrypting the user and server
     nonces that the Clarens server sends. This decryption works most of the
     time, but not always. This is an evil hack to work around that problem. */
  int conn = clarens_connect(client, clarens_url);
  for(int i = 0; conn != 0 && i < 3; i++) {
    cerr << env.fault_string << endl;
    warn("connect", "couldn't connect, retrying...");
    xmlrpc_env_clean(&env);
    xmlrpc_env_init(&env);
    conn = clarens_connect(client, clarens_url);
  }
  return conn;
}

void ClarensBackend::disconnect() {
  if(client)
    clarens_disconnect(client);
}

int ClarensBackend::query(const string& q) {
//  if(!server_info) {
  if(!client || !client->connected) {
    warn("query", "not connected");
    return -1;
  }

  xmlrpc_value* value = clarens_call(client, "boss.executeUpdate", "(s)", q.c_str());
  if(env.fault_occurred) {
    warn("query", env.fault_string);
    xmlrpc_env_clean(&env);
    return -1;
  }

  int status;
  xmlrpc_parse_value(&env, value, "i", &status);
  xmlrpc_DECREF(value);
  return status;
}

ResultSet ClarensBackend::fetch_query(const string& q) {
  if(!client || !client->connected) {
    warn("fetch_query", "not connected");
    return ResultSet(NULL);
  }

  xmlrpc_value* value = clarens_call(client, "boss.SQL", "(s)", q.c_str());
  if(env.fault_occurred) {
    warn("fetch_query", env.fault_string);
    xmlrpc_env_clean(&env);
    return ResultSet(NULL);
  }

  ResultSet results = ResultSet(new Clarens_ResultSetData(value));
  xmlrpc_DECREF(value);
  return results;
}

int ClarensBackend::rowcount_query(const string& q) {
  if(!client || !client->connected) {
    warn("rowcount_query", "not connected");
    return -1;
  }

  xmlrpc_value* value = clarens_call(client, "boss.existDataForQuery", "(s)", q.c_str());
  if(env.fault_occurred) {
    warn("rowcount_query", env.fault_string);
    xmlrpc_env_clean(&env);
    return -1;
  }

  int rowcount;
  xmlrpc_parse_value(&env, value, "i", &rowcount);
  xmlrpc_DECREF(value);
  return rowcount;
}

int ClarensBackend::insert_query(const string& q) {
  if(!client || !client->connected) {
    warn("insert_query", "not connected");
    return -1;
  }

  xmlrpc_value* value = clarens_call(client, "boss.executeInsert", "(s)", q.c_str());
  if(env.fault_occurred) {
    warn("insert_query", env.fault_string);
    xmlrpc_env_clean(&env);
    return -1;
  }

  int id;
  xmlrpc_parse_value(&env, value, "i", &id);
  xmlrpc_DECREF(value);
  return id;
}

string ClarensBackend::escape(string s) {
  cout << "escaping " << s << endl;
  return escape(s.data(), s.length());
}

string ClarensBackend::escape(const char* s, size_t len) {
  string esc;
  esc.reserve(2*len);
  for(unsigned int i = 0; i < len; i++) {
    if(s[i] == '\\')
      esc += "\\\\";
    else if(s[i] == '\0')
      esc += "\\0";
    else if(s[i] == '\'')
      esc += "\\'";
    else
      esc += s[i];
  }
  return esc;
}

string ClarensBackend::errormsg() {
  if(env.fault_occurred) {
    string e(env.fault_string);
    xmlrpc_env_clean(&env);
    return e;
  }
  else if(!client || !client->connected)
    return "not connected";
  else
    return "";
}


/***** Clarens_ResultSetData *****/

Clarens_ResultSetData::Clarens_ResultSetData(xmlrpc_value* value_) {
  value = value_;
  xmlrpc_INCREF(value);
  xmlrpc_env_init(&env);

  /* We assume that the result is of exactly the form we expect, and so
     we do very little type checking. */
  xmlrpc_parse_value(&env, value, "(AA)", &xvheaders, &xvrows);
  nrows = xmlrpc_array_size(&env, xvrows);
  nfields = xmlrpc_array_size(&env, xvheaders);

  parsed = false;
}

Clarens_ResultSetData::~Clarens_ResultSetData() {
  xmlrpc_DECREF(value);

  nfields = headers.size();
  nrows = rows.size();
  for(unsigned int j = 0; j < nfields; j++) {
    free(headers[j]);
    for(unsigned int i = 0; i < nrows; i++)
      free(rows[i][j]);
  }
}

const vector<vector<char*> >* Clarens_ResultSetData::getrows() {
  if(!parsed)
    parse();
  return &rows;
}

const vector<char*>* Clarens_ResultSetData::getheaders() {
  if(!parsed)
    parse();
  return &headers;
}

/* Helper function. Returns a string representation of the given xmlrpc_value.
   When finished with this string, it must be freed with free(). */
char* tostring(xmlrpc_env* env, xmlrpc_value* value) {
  char buf[64];         // long enough for ints and doubles
  int i;
  double d;
  char* s;

  switch(xmlrpc_value_type(value)) {
    case XMLRPC_TYPE_INT:
      xmlrpc_parse_value(env, value, "i", &i);
      snprintf(buf, 64, "%d", i);
      return strndup(buf, 64);
    case XMLRPC_TYPE_BOOL:
      xmlrpc_parse_value(env, value, "b", &i);
      snprintf(buf, 64, "%d", i);
      return strndup(buf, 64);
    case XMLRPC_TYPE_DOUBLE:
      xmlrpc_parse_value(env, value, "d", &d);
      snprintf(buf, 64, "%f", d);
      return strndup(buf, 64);
    case XMLRPC_TYPE_STRING:
      xmlrpc_parse_value(env, value, "s", &s);
      return strdup(s);
    default:
      return strdup("unknown type");
  }
}

void Clarens_ResultSetData::parse() {
  xmlrpc_value* xvh;
  char* h;
  headers.resize(nfields);
  for(unsigned int j = 0; j < nfields; j++) {
    xvh = xmlrpc_array_get_item(&env, xvheaders, j);
    xmlrpc_parse_value(&env, xvh, "s", &h);
    headers[j] = strdup(h);
  }

  xmlrpc_value* xvrow;
  xmlrpc_value* xve;
  char* e;
  rows.resize(nrows);
  for(unsigned int i = 0; i < nrows; i++) {
    xvrow = xmlrpc_array_get_item(&env, xvrows, i);
    rows[i].resize(nfields);
    for(unsigned int j = 0; j < nfields; j++) {
      xve = xmlrpc_array_get_item(&env, xvrow, j);
      e = tostring(&env, xve);
      rows[i][j] = e;
    }
  }

  parsed = true;
}
