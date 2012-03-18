#ifndef DBORCATASK_H
#define DBORCATASK_H
#include "Log.hh"

#include "DbTask.hh"

class DbOrcaTask : public DbTask {
public:
  DbOrcaTask(const int& anId) : DbTask(anId) {};
  int split();
};
#endif
