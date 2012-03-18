// /////////////////////////////////////////////////////////////////////
//
//                           CladLite
//             Lightweight Classified Advertisement
//
//   File:ClassAdLite.h
//   Version: 1.0
//   Author: Claudio Grandi (INFN Bologna)
//   Date: 3-May-2004
//   Change Log:
//
// /////////////////////////////////////////////////////////////////////

#ifndef CLASS_AD_LITE_H
#define CLASS_AD_LITE_H

#include <string>
#include <bits/stl_pair.h>
#include <vector>
#include <map>
#include <iostream>

namespace CAL {
  typedef std::pair<std::string, std::string> ClassAdLiteAttribute;
  //  typedef std::vector<ClassAdLiteAttribute> ClassAdLite;
  typedef std::map< std::string, std::string > ClassAdLite;

  int read(ClassAdLite&, std::istream&);
  int read(ClassAdLite&, std::string&);
  int readFromFile(ClassAdLite&, std::string);
  int add(ClassAdLite&, std::istream&);
  int add(ClassAdLite&, std::string&);
  int addFromFile(ClassAdLite&, std::string);
  int addAttr(ClassAdLite&, const std::string&);
  std::string nextAttr(std::istream&);
  //  std::string nextAttr(std::istream&, std::streampos&);
  int removeAttr(ClassAdLite&, std::string);
  int removeAttr(ClassAdLite&, ClassAdLite::iterator);
  void write(const ClassAdLite&, std::ostream&);
  void writeToFile(const ClassAdLite&, std::string);
  //ClassAdLite::iterator find(ClassAdLite&, std::string);
  //ClassAdLite::const_iterator find(const ClassAdLite&, std::string);
  void lookup(const ClassAdLite&, std::string, std::string&);
  void pop(ClassAdLite&, std::string, std::string&);
  std::vector<std::string> getList(std::string);
  std::string list2string(const std::vector<std::string>&);

  void removeOuterQuotes(std::string&);
  void clear(ClassAdLite&);

//   class equivCALAttr {
//   private:
//     ClassAdLiteAttribute a_;
//   public:
//     equivCALAttr(const ClassAdLiteAttribute& attr) : a_(attr) {}
//     bool operator() (const ClassAdLiteAttribute& attr) {
//       return attr.first == a_.first;
//     }
//  };

};

#endif
