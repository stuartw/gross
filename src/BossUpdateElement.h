// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 1.0
// File:    BossUpdateElement.h
// Authors: Claudio Grandi (INFN BO)
// Date:    3/10/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_UPDATE_ELEMENT_H
#define BOSS_UPDATE_ELEMENT_H

#include <string>
#include <iostream>

class BossUpdateElement { 

private:
  int jobid_;
  std::string table_;
  std::string varname_;
  std::string varvalue_;
public:
  static const char separator;
  BossUpdateElement(int, std::string, std::string, std::string); 
  BossUpdateElement(const BossUpdateElement&);
  BossUpdateElement(const std::string);  
  ~BossUpdateElement();
  BossUpdateElement& operator=(const BossUpdateElement&);
  BossUpdateElement& operator=(const std::string);
  int jobid() const { return jobid_; }
  std::string table() const { return table_; }
  std::string varname() const { return varname_; }
  std::string varvalue() const { return varvalue_; }
  std::string str() const;
  bool sameElement(const BossUpdateElement&);
};

std::ostream& operator<<(std::ostream&, const BossUpdateElement&);

class equivUpdateElement {
private:
  BossUpdateElement r_;
public:
  equivUpdateElement(const BossUpdateElement& e) : r_(e) {}
  bool operator() (const BossUpdateElement& e) {
    return e.jobid() == r_.jobid() && e.table() == r_.table() && 
      e.varname() == r_.varname();
  }
};
#endif


