#include "Range.hh"

#include <string>
#include <iostream>
#include <sstream>
#include "Job.hh"

Range::Range(string sIn, string sep_/*="-"*/) : min_(0), max_(0) {
  std::string::size_type idx = sIn.find(sep_);
  if(idx==std::string::npos) { //Single number, not range
    max_ = min_ = atoi(sIn.c_str());
  }
  else {
    min_=atoi((sIn.substr(0,idx)).c_str());
    max_=atoi((sIn.substr(idx+1)).c_str());
  }
  if(min_>max_) {
    int tmp = max_;
    max_ = min_;
    min_ = tmp;
  }
}
Range::Range(int minI, int maxI /*=0*/) : min_(minI), max_(maxI){}


bool JobRange::operator()(Job* pJob) const { //Functor for sorting thru job vector and getting subset of jobs with right id
  if(max_==0) {return false;} //Special case - all jobs returned if min_=max_=0
  if(pJob->Id()>=min_&&pJob->Id()<=max_) {return false;} 
  else return true;
}
