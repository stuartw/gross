// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossScheduler.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_SCHEDULER_H
#define BOSS_SCHEDULER_H

#include <string>
#include <map>

class BossJob;
class BossDatabase;

class BossScheduler {
private:
  BossDatabase* db_;

  typedef std::map< std::string,std::string,std::less<std::string> > Jobs;
  typedef Jobs::const_iterator Jobs_const_iterator;
  typedef Jobs::iterator Jobs_iterator;
  Jobs jobs_;
  time_t MAXDLY;
  typedef std::map< std::string, time_t, std::less<std::string> > LTM;
  typedef LTM::const_iterator LTM_const_iterator;
  typedef LTM::iterator LTM_iterator;
  LTM lastTimes_;
  std::string DELIM;
  void check(std::string);
  void clear(std::string sch="");

public:

  BossScheduler(BossDatabase*);
  ~BossScheduler();

  // Session Methods
  int submit(BossJob* jobH, std::string exe_host);
  int kill(BossJob* jobH, bool force);
  std::string query(BossJob* jobH);
  std::string status(BossJob* jobH, bool flag=false);
};

#endif

