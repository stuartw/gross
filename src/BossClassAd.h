// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossClassAd.h
// Authors: Claudio Grandi (INFN BO)
// Date:    15/07/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_CLASS_AD_H
#define BOSS_CLASS_AD_H

#include <string>

#include "ClassAdLite.h"

class BossClassAd {
private:
  CAL::ClassAdLite classad_;
public:
  typedef CAL::ClassAdLite::iterator iterator;
  typedef CAL::ClassAdLite::const_iterator const_iterator;
  BossClassAd() {}
  BossClassAd(std::istream& str) {readClad(str);}
  //BossClassAd(std::string file) {readClad(file);}
  ~BossClassAd() {}
  int readCladFromFile(std::string file) {return CAL::readFromFile(classad_,file);}
  int readClad(std::istream& cladstr) {return CAL::read(classad_,cladstr);}
  void dumpClad(std::string file) {CAL::writeToFile(classad_,file);}
  int addExpr(std::string key ,std::string value) {
    std::string str = key + "=" + value;
    return CAL::addAttr(classad_,str);
  }
  void resetClad() {CAL::clear(classad_);}; 
  void ClAdLookup(std::string key, std::string* value) {
    CAL::lookup(classad_,key,*value);
    //added by stuart
    CAL::removeOuterQuotes(*value);
  }
  iterator begin() { return classad_.begin(); }
  const_iterator begin() const { return classad_.begin(); }
  iterator end() { return classad_.end(); }
  const_iterator end() const { return classad_.end(); }
  static std::string getIdent(const_iterator it) { return it->first; }
  static std::string getValue(const_iterator it) { return it->second; }
private:
  BossClassAd(const BossClassAd& clad) {}
  BossClassAd& operator=(const BossClassAd& clad) {return *this;}
};
#endif

