// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossDatabase.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_DATABASE_H
#define BOSS_DATABASE_H

#include <iostream>
#include <vector>
#include <string>
#include <mysql.h>

class BossJob;

class BossDatabase { 
protected:
  char* hostname_;
  char* domain_;
  char* super_user_;
  char* guest_user_;
  char* super_passwd_;
  char* guest_passwd_;
  char* db_;
  unsigned int port_;
  char* unix_socket_;
  unsigned int client_flag_;
  std::string table_type_;
  std::string mode_;
  time_t last_err_time_;
  MYSQL* current_connection_;
public:
  BossDatabase(std::string);
  ~BossDatabase();

  int existJob(int id);
  int existJobType(std::string);
  int existSchType(std::string);
  std::string getWorkDir(std::string);
  std::string getDefaultSch();
  int insertJob(BossJob* JobH);
  int deleteJob(int);
  int deleteSpecificJob(std::string, int);
  BossJob* findJob(int);
  int getGeneralJobData(BossJob*);
  int getSpecificJobData(BossJob*);
  int updateJobParameter(int, std::string, std::string, std::string);
  int existColumn(std::string, std::string);
  int registerScheduler(std::string, std::string, std::string, 
			std::string, std::string, std::string);
  int registerJob(std::string, std::string, std::string, std::string, 
                  std::string, std::string);
  int deleteScheduler(std::string);
  int deleteJobType(std::string);
  std::string findSchema(std::string);
  int getSubmit(std::string, std::string);
  int getKill(std::string, std::string);
  int getQuery(std::string, std::string);
  int getPreProcess(std::string, std::string);
  int getRuntimeProcess(std::string, std::string);
  int getPostProcess(std::string, std::string);
  int SQLquery(std::string, std::ostream&);

  std::vector<std::string> schedulers();
  std::vector<std::string> jobTypes();
  std::vector<BossJob*> jobs(char option='a', std::string type="", 
                        std::string user="", std::string format="normal");
private:
  int connect();
  int disconnect();
  int tryQuery(MYSQL*, std::string);
  int accept(std::string);
  int affectedRows();
  MYSQL_RES* getResults(std::string);
  int clearResults(MYSQL_RES*);
  void fatal(std::string, int);
  std::string escape_file(std::string, unsigned int);
  std::string wj(std::string, unsigned int, std::string);
  std::string DSLType2SQLType(std::string);
  std::string DSLData2SQLData(std::string, std::string);

};
#endif



