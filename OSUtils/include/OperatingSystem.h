// /////////////////////////////////////////////////////////////////////
// Program: Utilities
// Version: 1.0
// File:    OperatingSystem.cpp
// Authors: Claudio Grandi (INFN BO)
// Date:    15/03/2004
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef OPERATING_SYSTEM_H
#define OPERATING_SYSTEM_H

#include <sstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <cstdio>
#include <ctime>
#include <unistd.h>
#include "pstream.h"

namespace OSUtils {
  class Process;
  std::string getUserName();
  std::string getHostName();
  int getPid();
  std::string getCurrentDir();
  int changeDir(std::string);
  std::string getEnv(std::string);
  int setEnv(std::string, std::string);
  std::string expandEnv(const std::string&);
  time_t getTime();
  std::string getStrTime();
  std::string time2StrTime(time_t*);
  int dirExist(std::string);
  int fileExist(std::string);
  bool isMine(std::string);
  int fileSize(std::string, unsigned int*);
  int fileCopy(std::string, std::string);
  int makeDir(std::string);
  int fileRemove(std::string);
  int dirRemove(std::string);
  int fileChmod(std::string, std::string);
  std::string basename(const std::string);
  std::string dirname(const std::string);
  int makeFIFO(std::string,int);
  int shell(std::string);
  int forkProcess(OSUtils::Process*);
  int waitProcess(OSUtils::Process*, std::string option="");
  int waitProcessMaxTime(OSUtils::Process*, int);
  std::string checkProcess(OSUtils::Process*);
  int terminateProcess(OSUtils::Process*);
  std::vector<std::string> splitString(const std::string&,char);
  void trim(std::string&);
  void sleep(unsigned);
  int tar(std::string,std::string,const std::vector<std::string>&);
  int append(std::string, std::string);
  std::string which(std::string);
  int touch(std::string);
  char* string2char(std::string);
  int string2int(std::string);
  //  void getCommandOutput(std::string, std::iostream&);
  template <typename T>
  std::string convert2string(T n) {
    std::ostringstream os;
    os<<n;
    return os.str();
  }
  typedef redi::ipstream CommandStream;
}

#endif
