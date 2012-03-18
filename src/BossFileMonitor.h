// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 1.0
// File:    BossFileMonitor.h
// Authors: Claudio Grandi (INFN BO)
// Date:    3/10/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_FILE_MONITOR_H
#define BOSS_FILE_MONITOR_H

#include <string>
#include <iostream>
#include <fstream>
#include "OperatingSystem.h"

class BossFileMonitor { 

private:
  std::string fname_;
  std::ifstream* str_;
  int lastline_;
  int startreadingline_;
public:

  explicit BossFileMonitor(std::string fname) 
    : fname_(fname), str_(0), lastline_(-1), startreadingline_(-1) {
    //cerr << "BossFileMonitor::BossFileMonitor(" << fname << ")" << endl;
  }

  ~BossFileMonitor() {
//      if ( str_ )
//        close();
  }

  int newlines(std::ostream& osl, bool& stop_loop) {
    //cerr << "BossFileMonitor::newlines" << endl;
    int ret_val = 0;
    if ( open()==0 ) {
      int counter=-1;
      while ( !str_->eof() ) {
	std::string buffer;
	getline(*str_,buffer);
	//std::cout << counter << " <=> " << lastline_ << " <" << buffer << ">" << std::endl;
	if( ++counter >= lastline_ ) {
	  if ( !buffer.empty() ) {
	    if ( buffer == "[" )
	      startreadingline_=counter;
	    else if ( buffer == "]" )
	      stop_loop = true;
	    else
	      if ( startreadingline_ >= 0 )
		osl << buffer << std::endl;
	    lastline_++;
	    ret_val++;
	  }
	}
      }
      //cerr << "BossFileMonitor::newlines: number of new lines = " << ret_val << endl;
      close();
    }

    return ret_val;
  }

private:

  int open() {
    int return_val = 0;
    if ( OSUtils::fileExist(fname_) ) {
      str_ = new std::ifstream(fname_.c_str());
      if ( str_ ) {
	if ( !(*str_) ) {
	  std::cerr << "BossFileMonitor::open(): Unable to access " << fname_ << std::endl;
	  return_val = -3;
	  str_ = 0;
	}
      } else {
	std::cerr << "BossFileMonitor::open(): Unable to open " << fname_ << std::endl;
	return_val = -2;
      }	
    } else {
      std::cerr << "BossFileMonitor::open(): " << fname_ << " not found" << std::endl;
      return_val =  -1;
    }
    return return_val;
  }

  int close() {
    int return_val = 0;
    if ( str_ ) {
      str_->close();
      delete str_;
      str_ = 0;
    } else {
      std::cerr << "BossFileMonitor::close(): " << fname_ << " is not open" << std::endl;
      return_val = -2;
    }
    return return_val;
  }

};

#endif


