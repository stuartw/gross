// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossOperatingSystem.cpp
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_OPERATING_SYSTEM_H
#define BOSS_OPERATING_SYSTEM_H

#include <sstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <cstdio>
#include <ctime>
#include <unistd.h>

class BossProcess;

class BossOperatingSystem {

private:
  static BossOperatingSystem* instance_;
  BossOperatingSystem();
  

public:

  // Constructor & Destructor
  // Nothing
  static BossOperatingSystem* instance();
  ~BossOperatingSystem();

  // Methods
  std::string getUserName();
  std::string getHostName();
  int getPid();
  std::string getCurrentDir();
  int changeDir(std::string);
  std::string getEnv(std::string);
  int setEnv(std::string, std::string);
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
  int forkProcess(BossProcess*);
  int waitProcess(BossProcess*, std::string option="");
  int waitProcessMaxTime(BossProcess*, int);
  void terminateProcess(BossProcess*);
  std::vector<std::string> splitString(std::string,char);
  void trim(std::string&);
  void sleep(unsigned);
  int tar(std::string,std::string,const std::vector<std::string>&);
  int append(std::string, std::string);
  std::string which(std::string);
  int touch(std::string);
  char* string2char(std::string);
  int string2int(std::string);
  template <typename T>
    std::string convert2string(T n) {
    std::ostringstream os;
    os<<n;
    return os.str();
  }
};

#endif
