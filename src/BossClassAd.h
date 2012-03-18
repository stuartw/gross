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

#include <iostream>
#include <string>

#include "classad_distribution.h"

class BossClassAd {
private:
  ClassAd* classad_;
public:
  typedef ClassAd::iterator iterator;
  typedef ClassAd::const_iterator const_iterator;
  BossClassAd();
  BossClassAd(std::istream&);
  BossClassAd(std::string);
  ~BossClassAd();
  int readClad(std::string); // read clad from file
  int readClad(std::istream&); // read clad from file
  int dumpClad(std::string); // prints clad to file
  int dumpClad(std::ostream&); // prints clad to ostream
  int addExpr(std::string,std::string); // add expr to clad
  void resetClad(); // delete old clad and create a new one
  void ClAdLookup(std::string, std::string*); // removes and returns requested expr
  void NestedClAdLookup(std::string,std::vector< std::pair<std::string,std::string> >*); // removes and returns nested classads
  // Note: If an attribute "BossNamespace" is found in the nested classad, 
  //       its value is pre-pended to all other keys, with a "::" separator. 
  //       The BossNamespace attirbute itself isn't added to the returned list 
  void ClAdExtractFromExpr(std::string, std::string, std::string*); // returns val from expr
  iterator begin() { return classad_->begin(); }
  const_iterator begin() const { return classad_->begin(); }
  iterator end() { return classad_->end(); }
  const_iterator end() const { return classad_->end(); }
  static std::string getIdent(const_iterator it) { return it->first; }
  static std::string getValue(const_iterator it) { return expr2string(it->second); }
private:
  BossClassAd(const BossClassAd& clad) {}
  BossClassAd& operator=(const BossClassAd& clad) {return *this;}
  static std::string expr2string(ExprTree*);
  static std::string extract_token(int*, const std::string &);
  static std::string removeOuterQuotes(const std::string &);
  static std::string extractRequestedValue(ExprTree*, const std::string&);
};
#endif

