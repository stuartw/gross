#ifndef MYSQLDB_H
#define MYSQLDB_H
#include "Log.hh"

#include <mysql.h>
class DbSpec;

class MySQLDb {
public:
  MySQLDb(DbSpec* aDbSpec);
  ~MySQLDb();  
  int init();
  MYSQL* connection() {return current_connection_;};
private:
  int connect();
  int disconnect();
  MYSQL* current_connection_;
  DbSpec* dbSpec_;
};
#endif
