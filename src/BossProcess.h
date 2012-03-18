// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossProcess.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_PROCESS_H
#define BOSS_PROCESS_H

#include<string>

class BossProcess {
private:
  int pid_;
  std::string return_status_;
public:
  BossProcess() : pid_(-1), return_status_("unknown") {}
  void setPid(int pid) { pid_=pid; }
  int getPid() const { return pid_; }
  void setRetCode(std::string rc) { return_status_=rc; }
  std::string retCode() const { return return_status_; }
  void execute() { start(); }
  virtual int start()=0;
  virtual ~BossProcess() {}
};

#endif
