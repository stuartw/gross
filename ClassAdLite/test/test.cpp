// /////////////////////////////////////////////////////////////////////
//
//                           CladLite
//             Lightweight Classified Advertisement
//
//   File:test.cpp
//   Version: 1.0
//   Author: Claudio Grandi (INFN Bologna)
//   Date: 3-May-2004
//   Change Log:
//
// /////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>

#include "ClassAdLite.h"

int main(int argc, char** argv) {
  CAL::ClassAdLite clad;
  int err = CAL::readFromFile(clad,"test.clad");
  std::cout << "Return code of ClassAd read is " << err << std::endl;   

  CAL::writeToFile(clad,"testwrite.clad");

  std::string tmp;
  CAL::lookup(clad,"executable",tmp="");
  std::cout << " lookup of executable: " << tmp << std::endl;
  CAL::lookup(clad,"unknown",tmp="");
  std::cout << " lookup of unknown: " << tmp << std::endl;
  std::vector<std::string>::const_iterator it;
  std::vector<std::string>::iterator it2;
  std::vector<std::string> l;
  std::cout << "BossAttr list values:" << std::endl;
  CAL::lookup(clad,"BossAttr",tmp="");
  l = CAL::getList(tmp);
  for (it = l.begin(); it != l.end(); ++it) 
    std::cout << (*it) << std::endl;
  std::cout << "Removing quotes from list elements" << std::endl;
  for (it2 = l.begin(); it2 != l.end(); ++it2) 
    CAL::removeOuterQuotes(*it2);
  std::cout << "Now the list is:" << std::endl;
  for (it = l.begin(); it != l.end(); ++it) 
    std::cout << (*it) << std::endl;
  
  std::cout << "jobtype list values:" << std::endl;
  CAL::lookup(clad,"jobtype",tmp="");
  l = CAL::getList(tmp);
  for (it = l.begin(); it != l.end(); ++it) 
    std::cout << (*it) << std::endl;
  std::string Slist = CAL::list2string(l);
  std::cout << "test list2string: " << Slist << std::endl;

  return 0;
}
