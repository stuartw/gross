#ifndef RANGE_H
#define RANGE_H

#include "Log.hh"

class Job;

/*!
  \brief Helper class to define a range of numbers

*/

class Range {
public:
  Range(string sIn, string sep_="-");
  Range(int minI, int maxI = 0);
  int min() {return min_;};
  int max() {return max_;};
protected: 
  int min_;
  int max_;
private:
  string sep_;
};

class JobRange : public Range {
public:
  JobRange(int minJob, int maxJob) : Range(minJob, maxJob) {};
  bool operator()(Job* pJob) const; ///<Functor for getting subrange of jobs
};



#endif
