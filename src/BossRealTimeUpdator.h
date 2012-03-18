// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.0
// File:    BossRealTimeUpdator.h
// Authors: Claudio Grandi (INFN BO), Jordan Carlson (Caltech), 
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////
#ifndef BOSS_REAL_TIME_UPDATOR_H
#define BOSS_REAL_TIME_UPDATOR_H

#include <string>

#include "BossUpdateSet.h"

class SirDBBackend;


class BossRealTimeUpdator {
public:
  explicit BossRealTimeUpdator(int jobID = 0);
  ~BossRealTimeUpdator();

  bool connect();
  bool disconnect();

  /* Update the given job parameters. Return the number of updates
     successfully performed. */
  int update(const BossUpdateSet& us);

  /* Update a single job parameter. Return 1 on success, 0 on failure. */
  int updateJobParameter(int id, std::string tab, std::string key, std::string val);

  /* Return true if the specified column exists in the specified table. */
  bool column_exists(const std::string& column, const std::string& table);

private:
  SirDBBackend* backend;
  int jobid;
};

#endif // REAL_TIME_UPDATOR_H
