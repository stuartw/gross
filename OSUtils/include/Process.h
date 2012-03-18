// /////////////////////////////////////////////////////////////////////
// Program: Utilities
// Version: 1.0
// File:    Process.h
// Authors: Claudio Grandi (INFN BO)
// Date:    15/03/1004
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef PROCESS_H
#define PROCESS_H

#include<string>

namespace OSUtils {
  class Process {
  private:
    std::string name_;
    int pid_;
    std::string return_status_;
  public:
    explicit Process(std::string name = "unnamed") : 
      name_(name), pid_(-1), return_status_("unknown") {}
    void setName(std::string name) { name_=name; }
    void setPid(int pid) { pid_=pid; }
    std::string name() const { return name_; }
    int getPid() const { return pid_; }
    void setRetCode(std::string rc) { return_status_=rc; }
    std::string retCode() const { return return_status_; }
    void execute() { start(); }
    virtual int start()=0;
    virtual ~Process() {}
  };
}
#endif
