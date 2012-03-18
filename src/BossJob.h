// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossJob.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:    
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_JOB_H
#define BOSS_JOB_H

#include <iostream>
#include <string>
#include <vector>

#include "BossJobData.h"
#include "BossUpdateSet.h"

class BossDatabase;

class BossJob { 

private:
  BossJobData generalData_;
  BossJobData specificData_;
  std::string journalFile_;
  BossDatabase* db_;

public:

  // Access to parameters 
  typedef BossJobData::const_iterator const_iterator;
  typedef BossJobData::iterator iterator;

  BossJob() : journalFile_(""), db_(0) { initialize(); }

  // Set Updator mode
  void setUpdator(BossDatabase* db) { db_ = db; }
  void setUpdator(std::string file) { journalFile_ = file; }

  const_iterator beginGeneral() { return generalData_.begin(); }
  const_iterator endGeneral() { return generalData_.end(); }
  const_iterator beginSpecific() { return specificData_.begin(); }
  const_iterator endSpecific() { return specificData_.end(); }
  BossJobElement& specificData(iterator i) { return specificData_[i]; }
  BossJobElement& generalData(iterator i) { return generalData_[i]; }
  const BossJobElement& specificData(const_iterator i) { return specificData_[i]; }
  const BossJobElement& generalData(const_iterator i) { return generalData_[i]; }

  std::string getGeneralData(std::string) const;
  std::string getGeneralDataType(std::string) const;

  std::string getSpecificData(std::string) const;
  std::string getSpecificDataType(std::string) const;


  std::string getJobTypeString() const;
  std::vector<std::string> getJobTypes() const;
  int isOfType(std::string) const;
  int getId() const;
  std::string getSchedType() const;
  std::string getSid() const;
  std::string getSubHost() const;
  std::string getSubPath() const;
  std::string getSubUser() const;
  std::string getExecutable() const; 
  std::string getArguments() const;
  std::string getStdin() const ;
  std::string getStdout() const;
  std::string getStderr() const ;
  std::vector<std::string> getInFiles() const ;
  std::vector<std::string> getOutFiles() const ;
  std::string getInFilesString() const ;
  std::string getOutFilesString() const ;
  std::string getExeHost() const;
  std::string getExePath() const;
  std::string getExeUser() const;
  std::string getLog() const;
  std::string getRetCode() const;
  time_t getSubTime() const;
  time_t getStartTime() const;
  time_t getStopTime() const;
  std::string getStatTime() const;
  time_t getLastContactTime() const;

  void printGeneral(std::string output_type, std::string state="R") const;
  void printSpecific(std::string) const;

  void dumpGeneral(std::ostream& os=std::cout) const;
  void dumpSpecific(std::ostream& os=std::cout) const;

  // Parameter update methods  
  // Job param I/O through BossUpdateSet
  int setData(std::string fname, bool force = false);
  int setData(std::istream& usfile, bool force = false);
  int setData(const BossUpdateElement& us, bool force = false);
  int setData(const BossUpdateSet& us, bool force = false);

  void setBasicInfo(std::string,std::string,std::string,std::string,
                    std::string,std::string,std::string,std::string,
		    std::string);
  void setSubInfo(std::string,std::string,std::string);
  void setScheduler(std::string);
  void setExeInfo(std::string,std::string,std::string,time_t);
  void setId(int);
  void setSid(std::string);
  void setLog(std::string);
  void setSubTime(time_t);
  void setStopTime(time_t);
  void setStatTime(std::string);
  void setLastContactTime(time_t);
  void setRetCode(std::string);

  int generalData2UpdateSet(BossUpdateSet&) const;
  int specificData2UpdateSet(BossUpdateSet&) const;

  // Schema I/O through a string of the form name1:type1,name2:type2...
  void setSchema(std::string name, std::istream& str ) { specificData_.add(name, str); }
  std::string getSchema() const { return specificData_.getSchema(); }
  std::string getGeneralSchema() const { return generalData_.getSchema(); }

  bool existColumn(std::string, std::string) const;
  std::vector<std::string> whichTables(std::string) const;
private:
  void initialize();
  std::string wj(std::string data, unsigned int width, std::string justify="left") const;
  std::string str_time(const time_t*) const;
}; 

#endif
