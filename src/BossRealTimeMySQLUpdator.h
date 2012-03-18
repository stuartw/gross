// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 1.0
// File:    BossRealTimeMySQLUpdator.h
// Authors: Claudio Grandi (INFN BO)
// Date:    08/10/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_REAL_TIME_MYSQL_UPDATOR_H
#define BOSS_REAL_TIME_MYSQL_UPDATOR_H

#include "BossRealTimeUpdator.h"
#include <mysql.h>
#include <string>

class BossRealTimeMySQLUpdator : public BossRealTimeUpdator { 

private:
  char* hostname_;
  char* domain_;
  char* user_;
  char* passwd_;
  char* db_;
  unsigned int port_;
  char* unix_socket_;
  unsigned int client_flag_;
  time_t last_err_time_;
  MYSQL* current_connection_;

public:

  explicit BossRealTimeMySQLUpdator(int); 
  virtual ~BossRealTimeMySQLUpdator();
  virtual int updateJobParameter(int, std::string, std::string, std::string);
  virtual bool connect();
  virtual bool disconnect();

private:
  int tryQuery(MYSQL*, std::string);
  int accept(std::string);
  MYSQL_RES* getResults(std::string);
  int clearResults(MYSQL_RES*);
  void fatal(std::string, int);
  int existColumn(std::string, std::string);
  int existTable(std::string);
  char* parseInputString(std::string);
};
#endif

