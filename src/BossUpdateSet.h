// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 1.0
// File:    BossUpdateSet.h
// Authors: Claudio Grandi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_UPDATE_SET_H
#define BOSS_UPDATE_SET_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "BossUpdateElement.h"
#include "BossOperatingSystem.h"

class BossUpdateSet { 

public:

  typedef std::vector< BossUpdateElement > Data;
  typedef Data::const_iterator const_iterator;
  typedef Data::iterator iterator;

private:
  Data data_;

public:

  BossUpdateSet() {
  }

  BossUpdateSet(std::istream& is) {
    readUpdateSet(is);
  }

  BossUpdateSet(std::string file) {
    readUpdateSet(file);
  }


  bool add(const BossUpdateElement& elem) {
    bool ret_val = false;
    if (elem.jobid() != 0) {
      data_.push_back(elem);
      ret_val = true;
    }
    return ret_val;
  }

  bool remove(int jobid, std::string table, std::string name) {
    bool ret_val = false;
    BossUpdateElement match(jobid,table,name,"");
    iterator last = remove_if(begin(),end(),equivUpdateElement(match));
    data_.erase(last,end());
    ret_val = true;
    return ret_val;
  }

  int readUpdateSet(std::istream& is) {
    int return_val = 0;
    if (is) {
      while(!is.eof()) {
	std::string line = "";
	getline(is,line);
	if (line.size()<1)
	  continue;
	int start = 0;
	while (isspace(line[start]))
	  start++;   // skip spaces
	if ((line.substr(start,line.size()-start)).size()<1 || 
	    line[start]=='#') {
	  continue;  // skip enpty lines or starting with #
	}
	add(BossUpdateElement(line));
      }
    } else {
      std::cerr << "BossUpdateSet: Unable to read UpdateSet stream" << std::endl;
      return_val =  -2;
    }
    return return_val;
  }

  int readUpdateSet(std::string file) {
    int return_val = 0;
    BossOperatingSystem* sys=BossOperatingSystem::instance();
    if ( sys->fileExist(file) ) {
      std::ifstream us(file.c_str());
      if ( us ) {
	return_val = readUpdateSet(us);
      } else {
	std::cerr << "BossUpdateSet: Unable to open UpdateSet file: " << file << std::endl;
	return_val = -2;
      }  
    } else {
      std::cerr << "BossUpdateSet: UpdateSet file doesn't exist" << std::endl;
      return_val =  -1;
    }
    return return_val;
  }

  int readFromFilterFile(int jobid, std::string table, std::istream& is) {
    int return_val = 0;
    BossOperatingSystem* sys=BossOperatingSystem::instance();
    if (is) {
      while(!is.eof()) {
	std::string buffer;
	char ch = is.peek();
	while ( ch == '#' || ch == '\n' ) { // skip comment and empty lines
	  getline(is,buffer);
	  ch = is.peek();
	}
	if ( ch == EOF ) break; // exit if OEF
	getline(is,buffer,'=');
	sys->trim(buffer);
	std::string ident = buffer;
	if ( buffer == "" ) continue;
	getline(is,buffer,'\n');
	sys->trim(buffer);
	std::string value = buffer;
	if ( buffer == "" ) continue;
	add(BossUpdateElement(jobid,table,ident,buffer));
      }
    } else {
      std::cerr << "BossUpdateSet: Unable to read UpdateSet stream" << std::endl;
      return_val =  -2;
    }
    return return_val;
  }

  int readFromFilterFile(int jobid, std::string table, std::string file) {
    int return_val = 0;
    BossOperatingSystem* sys=BossOperatingSystem::instance();
    if ( sys->fileExist(file) ) {
      std::ifstream us(file.c_str());
      if ( us ) {
	return_val = readFromFilterFile(jobid,table,us);
      } else {
	std::cerr << "BossUpdateSet: Unable to open filter file: " << file << std::endl;
	return_val = -2;
      }  
    } else {
      std::cerr << "BossUpdateSet: filter file doesn't exist" << std::endl;
      return_val =  -1;
    }
    return return_val;
  }

  

  ~BossUpdateSet() {}

  void dump(std::string file) const {
    std::ofstream usfile(file.c_str());
    if (usfile) {
      dump(usfile);
     } else {
       std::cerr << "BossUpdateSet::dump: unable to open file " << file << std::endl;
     }
  }

  void dump(std::ostream& os) const {
    //os << "[" << std::endl;
    for (const_iterator it=begin(); it<end(); it++)
      os << (*it) << std::endl;
    //os << "]" << std::endl;
  }

  const_iterator begin() const { return data_.begin(); }
  const_iterator end() const { return data_.end(); }
  iterator begin() { return data_.begin(); }
  iterator end() { return data_.end(); }
  int size() const { return data_.size(); }

};

#endif
