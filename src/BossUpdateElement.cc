// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 1
// File:    BossUpdateElement.cc
// Authors: Claudio Grandi (INFN BO)
// Date:    09/10/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include "BossUpdateElement.h"
#include <iostream>
#include "OperatingSystem.h"

using namespace std;

const char BossUpdateElement::separator = '\t';

BossUpdateElement::BossUpdateElement(int id=0, string tab="", 
				     string name="", string value="") 
  : jobid_(id), table_(tab), varname_(name), varvalue_(value) {}

BossUpdateElement::BossUpdateElement(const BossUpdateElement& e) {
  jobid_ = e.jobid_;
  table_ = e.table_;
  varname_ = e.varname_;
  varvalue_ = e.varvalue_;
}

BossUpdateElement::BossUpdateElement(const string buffer) {
  jobid_ = 0;
  table_ = "";
  varname_ = "";
  varvalue_ = "";
  bool error = false;
  int delim[5] = {0,0,0,0,0};
  delim[4] = buffer.size();
  while (isspace(buffer[delim[0]]))
    delim[0]++;                         // skip spaces
  // if the first character is [ or ] skip processing
  if ( buffer[delim[0]] != '[' && buffer[delim[0]] != ']' ) {
    delim[0] -= 1; // re-align
    //cerr << "BossUpdateElement::BossUpdateElement(string): Delimiters are: " << delim[0];
    for (int i=1; i<4; i++ ) {
      delim[i] = buffer.find_first_of(BossUpdateElement::separator,
				      delim[i-1]+1);
      //cerr << " " << delim[i];
      if ( delim[i]-delim[i-1] <= 1 ||  delim[i] >= delim[4] ) {
	error = true;
	break;
      }
    }
    //cerr << " " << delim[4] << endl;
    if ( !error ) {
      jobid_ = atoi(buffer.substr(delim[0]+1,
				  delim[1]-delim[0]-1).c_str());
      if ( jobid_ == 0 ) {
	cerr << "BossUpdateElement::BossUpdateElement: Wrong job identifier: " 
	     << buffer.substr(delim[0]+1,delim[1]-delim[0]-1) << endl;
      } else {
	table_ =      buffer.substr(delim[1]+1,
				    delim[2]-delim[1]-1);
	varname_ =    buffer.substr(delim[2]+1,
				    delim[3]-delim[2]-1);
	varvalue_ =   buffer.substr(delim[3]+1,
				    delim[4]-delim[3]-1);
	//cerr << "BossUpdateElement::BossUpdateElement(string):" 
	//     << " " << jobid_
	//     << " " << table_
	//     << " " << varname_
	//     << " " << varvalue_
	//     << endl;
      }
    } else {
      cerr << "BossUpdateElement::BossUpdateElement: Wrong format: " 
	   << endl << buffer << endl;
    }
  }
}

BossUpdateElement::~BossUpdateElement() {}

BossUpdateElement& BossUpdateElement::operator=(const BossUpdateElement& e) {
  jobid_ = e.jobid_;
  table_ = e.table_;
  varname_ = e.varname_;
  varvalue_ = e.varvalue_;
  return *this;
}

BossUpdateElement& BossUpdateElement::operator=(const string buffer) {
  BossUpdateElement e(buffer);
  jobid_ = e.jobid_;
  table_ = e.table_;
  varname_ = e.varname_;
  varvalue_ = e.varvalue_;
  return *this;
}

string BossUpdateElement::str() const {
  string strid = OSUtils::convert2string(jobid_);
  return strid+BossUpdateElement::separator+
    table_+BossUpdateElement::separator+
    varname_+BossUpdateElement::separator+
    varvalue_;
}

ostream& operator<<(ostream& os, const BossUpdateElement& e) {
  os << e.str();
  return os;
}
