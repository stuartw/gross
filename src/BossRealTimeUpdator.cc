// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 1.0
// File:    BossRealTimeUpdator.cc
// Authors: Claudio Grandi (INFN BO), Jordan Carlson (Caltech), 
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////
#include <sstream>

#include "BossUpdateSet.h"
#include "SirDBBackend.h"
#include "BossRealTimeUpdator.h"
#include "BossConfiguration.h"

using namespace std;


BossRealTimeUpdator::BossRealTimeUpdator(int jobID) : jobid(jobID) {
  BossConfiguration* config=BossConfiguration::instance();
  backend = SirDBBackend::create(config->boss_config_file(),"super");
}

BossRealTimeUpdator::~BossRealTimeUpdator() {
}

bool BossRealTimeUpdator::connect() {
  return (backend->connect() == 0);
}

bool BossRealTimeUpdator::disconnect() {
  backend->disconnect();
  return true;
}

int BossRealTimeUpdator::update(const BossUpdateSet& uset) {
  int num = 0;
  for(BossUpdateSet::const_iterator u = uset.begin(); u != uset.end(); u++) {
    if(u->jobid() == jobid)
      num += updateJobParameter(jobid, u->table(), u->varname(), u->varvalue());
  }
  return num;
}

int BossRealTimeUpdator::updateJobParameter(int id, string tab, string key, string val) {
  string idn = (tab == "JOB") ? "ID" : "JOBID";
  stringstream qs;
  qs << "UPDATE " << tab << " SET " << key << "='" << val << "' WHERE "
     << idn << "=" << id;
  if(column_exists(key, tab) && backend->query(qs.str()) == 0)
    return 1;
  else
    return 0;
}

bool BossRealTimeUpdator::column_exists(const string& column, const string& table) {
  /* First see if the table exists */
  bool table_exists = false;
  SirDBResultSet results = backend->fetch_query("SHOW TABLES");
  size_t nrows = results.nrows();
  for(unsigned int i = 0; i < nrows; i++) {
    if(table == string(results[i][0]))
      table_exists = true;
  }
  if(!table_exists)
    return false;

  /* Now see if the column exists within that table */
  results = backend->fetch_query("SHOW FIELDS FROM " + table);
  nrows = results.nrows();
  for(unsigned int i = 0; i < nrows; i++) {
    if(column == string(results[i][0]))
      return true;
  }
  return false;
}
