// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 1.0
// File:    BossJobIDRange.h
// Authors: Claudio Grandi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_JOB_ID_RANGE_H
#define BOSS_JOB_ID_RANGE_H

#include<iostream>
#include<string>
#include<vector>
#include "OperatingSystem.h"
#include<limits>

class BossJobIDRange {
private:
  int i0_;
  int i1_;
public:
  BossJobIDRange(std::string range) : i0_(0), i1_(-1) {
    updateRange(range);
  }
  void updateRange(std::string range) {
    std::vector<std::string> limits = OSUtils::splitString(range,':');
    int i0=0;
    int i1=0;
    if (limits.size() ==1) {
      i0 = OSUtils::string2int(limits[0]);
      i1 = OSUtils::string2int(limits[0]);
    } else if (limits.size() ==2) {
      i0 = OSUtils::string2int(limits[0]);
      i1 = OSUtils::string2int(limits[1]);
//       if (i0_>i1_) {
// 	int tmp = i1_;
// 	i1_ = i0_;
// 	i0_ = tmp;
//       }
    } else {
      std::cerr << "Specify Job ID range either as a single ID or as \"firstID:lastID\"" << std::endl;
    }
    // DEBUG
    //std::cout << "ID range input is " << range << "; output is " << i0 << ":" << i1 << std::endl;
    // END DEBUG
    if (i0>0 && i1>0) {
      i0_=i0;
      i1_=i1;
    } 
  }  
  int size() const { return i1_-i0_+1; }
  int ifirst() {return  i0_;}
  int ilast() {return i1_;}
  std::string sfirst() {
    return  OSUtils::convert2string(i0_);
  }
  std::string slast() {
    return OSUtils::convert2string(i1_);
  }
  static const int maxJobID() {return std::numeric_limits<int>::max();}
};

#endif
