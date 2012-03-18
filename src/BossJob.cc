// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossJob.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:    
// /////////////////////////////////////////////////////////////////////

#include "BossJob.h"

#include "OperatingSystem.h"
#include "BossDatabase.h"
#include "BossUpdateElement.h"
#include "ClassAdLite.h"

#include <cstring>
#include <map>
#include <fstream>
#include <iostream>
#include <ctime>

using namespace std;

void BossJob::initialize() {

  generalData_.add("JOB","ID"       ,"INT"    );
  generalData_.add("JOB","TYPE"     ,"VARCHAR");
  generalData_.add("JOB","SCH"      ,"VARCHAR");
  generalData_.add("JOB","SID"      ,"VARCHAR");
  generalData_.add("JOB","S_HOST"   ,"VARCHAR");
  generalData_.add("JOB","S_PATH"   ,"VARCHAR");
  generalData_.add("JOB","S_USR"    ,"VARCHAR");
  generalData_.add("JOB","EXEC"     ,"VARCHAR");
  generalData_.add("JOB","ARGS"     ,"VARCHAR");
  generalData_.add("JOB","STDIN"    ,"VARCHAR");
  generalData_.add("JOB","STDOUT"   ,"VARCHAR");
  generalData_.add("JOB","STDERR"   ,"VARCHAR");
  generalData_.add("JOB","IN_FILES" ,"BLOB"   );
  generalData_.add("JOB","OUT_FILES","BLOB"   );
  generalData_.add("JOB","E_HOST"   ,"VARCHAR");
  generalData_.add("JOB","E_PATH"   ,"VARCHAR");
  generalData_.add("JOB","E_USR"    ,"VARCHAR");
  generalData_.add("JOB","LOG"      ,"VARCHAR");
  generalData_.add("JOB","T_SUB"    ,"INT"    );
  generalData_.add("JOB","T_START"  ,"INT"    );
  generalData_.add("JOB","T_STOP"   ,"INT"    );
  generalData_.add("JOB","T_STAT"   ,"VARCHAR");
  generalData_.add("JOB","RET_CODE" ,"VARCHAR");
  generalData_.add("JOB","T_LAST"   ,"INT"    );
}

string BossJob::getGeneralData(string name) const {
  BossJobData::const_iterator p = generalData_.find(name);
  if ( p != generalData_.end() ) {
    return generalData_[p].value(); 
  }else {
    return "";
  } 
}

string BossJob::getGeneralDataType(string name) const {
  BossJobData::const_iterator p = generalData_.find(name);
  if ( p != generalData_.end() ) {
    return generalData_[p].type(); 
  }else {
    return "";
  } 
}

string BossJob::getSpecificData(string name) const {
  BossJobData::const_iterator p = specificData_.find(name);
  if ( p != specificData_.end() ) {
    return specificData_[p].value();
  } else {
    return "";
  }
}

string BossJob::getSpecificDataType(string name) const {
  BossJobData::const_iterator p = specificData_.find(name);
  if ( p != specificData_.end() ) {
    return specificData_[p].type();
  } else {
    return "";
  }
}

vector<string> BossJob::getJobTypes() const { 
  vector<string> tmp = CAL::getList(getJobTypeString());
  for (vector<string>::iterator it = tmp.begin(); it != tmp.end(); ++it)
    CAL::removeOuterQuotes(*it);
  return tmp;
}

vector<string> BossJob::getInFiles() const { 
  vector<string> tmp = CAL::getList(getInFilesString());
  for (vector<string>::iterator it = tmp.begin(); it != tmp.end(); ++it)
    CAL::removeOuterQuotes(*it);
  return tmp;
}

vector<string> BossJob::getOutFiles() const { 
  vector<string> tmp = CAL::getList(getOutFilesString());
  for (vector<string>::iterator it = tmp.begin(); it != tmp.end(); ++it)
    CAL::removeOuterQuotes(*it);
  return tmp;
}

int BossJob::isOfType(string type) const {
  vector<string> types = getJobTypes();
  for(vector<string>::const_iterator ti=types.begin(); ti<types.end(); ti++) {
    if ( *ti == type )
      return 1;
  }
  return 0;
}

int BossJob::getId() const { 
  return atol(generalData_["ID"].value().c_str()); 
}

string BossJob::getSchedType() const { 
  return generalData_["SCH"].value(); 
}

string BossJob::getSid() const { 
  return generalData_["SID"].value(); 
}

string BossJob::getSubHost() const { 
  return generalData_["S_HOST"].value(); 
}

string BossJob::getSubPath() const { 
  return generalData_["S_PATH"].value(); 
}

string BossJob::getSubUser() const { 
  return generalData_["S_USR"].value(); 
} 

string BossJob::getExecutable() const { 
  return generalData_["EXEC"].value(); 
}

string BossJob::getArguments() const { 
  return generalData_["ARGS"].value(); 
}

string BossJob::getStdin() const { 
  return generalData_["STDIN"].value(); 
}

string BossJob::getStdout() const { 
  return generalData_["STDOUT"].value(); 
}

string BossJob::getStderr() const { 
  return generalData_["STDERR"].value(); 
}

string BossJob::getInFilesString() const { 
  return generalData_["IN_FILES"].value(); 
}

string BossJob::getOutFilesString() const { 
  return generalData_["OUT_FILES"].value(); 
}

string BossJob::getExeHost() const { 
  return generalData_["E_HOST"].value(); 
}

string BossJob::getExePath() const { 
  return generalData_["E_PATH"].value(); 
}

string BossJob::getExeUser() const { 
  return generalData_["E_USR"].value(); 
}

string BossJob::getLog() const { 
  return generalData_["LOG"].value(); 
}

string BossJob::getRetCode() const {
  return generalData_["RET_CODE"].value();
}

time_t BossJob::getSubTime() const { 
  return atol(generalData_["T_SUB"].value().c_str()); 
}

time_t BossJob::getStartTime() const { 
  return atol(generalData_["T_START"].value().c_str()); 
}

time_t BossJob::getStopTime() const { 
  return atol(generalData_["T_STOP"].value().c_str()); 
}

string BossJob::getStatTime() const { 
  return generalData_["T_STAT"].value(); 
}

time_t BossJob::getLastContactTime() const { 
  return atol(generalData_["T_LAST"].value().c_str()); 
}

string BossJob::getJobTypeString() const { 
  return generalData_["TYPE"].value(); 
}

void BossJob::setBasicInfo(string type, string exe, string args,
			   string stdin, string stdout, string stderr,
			   string infiles, string outfiles,
			   string log) {
  setData(BossUpdateElement(getId(),"JOB","TYPE",type),true);
  setData(BossUpdateElement(getId(),"JOB","EXEC",exe),true);
  setData(BossUpdateElement(getId(),"JOB","ARGS",args),true);
  setData(BossUpdateElement(getId(),"JOB","STDIN",stdin),true);
  setData(BossUpdateElement(getId(),"JOB","STDOUT",stdout),true);
  setData(BossUpdateElement(getId(),"JOB","STDERR",stderr),true);
  setData(BossUpdateElement(getId(),"JOB","IN_FILES",infiles),true);
  setData(BossUpdateElement(getId(),"JOB","OUT_FILES",outfiles),true);
  setData(BossUpdateElement(getId(),"JOB","LOG",log),true);
}

void BossJob::setSubInfo(string host, string path, string user) {
  setData(BossUpdateElement(getId(),"JOB","S_HOST",host),true);
  setData(BossUpdateElement(getId(),"JOB","S_PATH",path),true);
  setData(BossUpdateElement(getId(),"JOB","S_USR",user),true);
}

void BossJob::setScheduler(string sched) {
  setData(BossUpdateElement(getId(),"JOB","SCH",sched),true);
} 

void BossJob::setExeInfo(string host, string path, string user, time_t time) {
  setData(BossUpdateElement(getId(),"JOB","E_HOST",host),true);
  setData(BossUpdateElement(getId(),"JOB","E_PATH",path),true);
  setData(BossUpdateElement(getId(),"JOB","E_USR",user),true);
  setData(BossUpdateElement(getId(),"JOB","T_START",OSUtils::convert2string(time)),true);
}

void BossJob::setId(int id) {
  generalData_["ID"]      = OSUtils::convert2string(id); 
}

void BossJob::setSid(string sid) { 
  setData(BossUpdateElement(getId(),"JOB","SID",sid),true);
}

void BossJob::setLog(string log) { 
  setData(BossUpdateElement(getId(),"JOB","LOG",log),true);
}

void BossJob::setSubTime(time_t time) { 
  setData(BossUpdateElement(getId(),"JOB","T_SUB",OSUtils::convert2string(time)),true);
}

void BossJob::setStopTime(time_t time) { 
  setData(BossUpdateElement(getId(),"JOB","T_STOP",OSUtils::convert2string(time)),true);
}

void BossJob::setStatTime(string time) { 
  setData(BossUpdateElement(getId(),"JOB","T_STAT",time),true);
}

void BossJob::setLastContactTime(time_t time) { 
  setData(BossUpdateElement(getId(),"JOB","T_LAST",OSUtils::convert2string(time)),true);
}

void BossJob::setRetCode(string code) {
  setData(BossUpdateElement(getId(),"JOB","RET_CODE",code),true);
}

int BossJob::setData(string fname, bool force) {
  int ret_val = 0;
  if (OSUtils::fileExist(fname)) {
    ifstream usfile(fname.c_str());
    if (usfile) {
      ret_val = setData(usfile, force);
    } else {
      cerr << "BossJob: unable to read from UpdateSet file: " << fname << endl;
      ret_val = -2;
    }
  } else {
    cerr << "BossJob: unable to find UpdateSet file: " << fname << endl;
    ret_val = -1;
  }
  return ret_val;
}

int BossJob::setData(istream& usfile, bool force) {
  int ret_val = 0;
  if (usfile) {
    BossUpdateSet us(usfile);
    ret_val = setData(us, force);
  } else {
    cerr << "BossJob: unable to read from UpdateSet file" << endl;
    ret_val = -2;
  }
  return ret_val;
}

int BossJob::setData(const BossUpdateSet& us, bool force) { 
  int ret_val = 0;
  for (BossUpdateSet::const_iterator it=us.begin(); it!=us.end(); it++)
    ret_val += setData(*it, force);
  return ret_val;
}

int BossJob::setData(const BossUpdateElement& elem, bool force) {
  int ret_val = -1;
  BossJobData* data = (elem.table()=="JOB") ? &generalData_ : &specificData_;
  if( force || data->existColumn(elem.table(),elem.varname()) ) {
    data->assign(elem.varname(),elem.varvalue()); 
    ret_val = 0;
    if (db_)
      db_->updateJobParameter(elem.jobid(),elem.table(),
			      elem.varname(),elem.varvalue() );
    if (OSUtils::fileExist(journalFile_))
      OSUtils::append(journalFile_,elem.str());
  } else {
    cerr << "BossJob::setData: no such parameter: " 
	 << elem.table() << "." << elem.varname()  
	 << " for job " << elem.jobid() << "!" << endl;
  }
  return ret_val;
}

int BossJob::generalData2UpdateSet(BossUpdateSet& us) const {
  BossJobData::const_iterator p;
  int counter = 0;
  for (p=generalData_.begin(); p!=generalData_.end(); p++) {
    string key = generalData_[p].name();
    string val = generalData_[p].value();
    us.add(BossUpdateElement(getId(),"JOB",key,val));
    counter++;
  }
  return counter;
}

int BossJob::specificData2UpdateSet(BossUpdateSet& us ) const {
  BossJobData::const_iterator p;
  int counter = 0;
  for (p=specificData_.begin(); p!=specificData_.end(); p++) {
    string key = specificData_[p].name();
    string val = '"' + specificData_[p].value() + '"';
    vector<string> types = whichTables(key);
    for(vector<string>::const_iterator ti=types.begin(); ti<types.end(); ti++) {
      us.add(BossUpdateElement(getId(),(*ti),key,val));
      counter++;
    }
  }
  return counter;
}

void BossJob::dumpGeneral(ostream& os) const{
  time_t time;
  os << "ID       = " << getId() << endl;
  os << "TYPE     = " << getJobTypeString() << endl;
  os << "SCH      = " << getSchedType() << endl;
  os << "SID      = " << getSid() << endl;
  os << "S_HOST   = " << getSubHost() << endl;
  os << "S_PATH   = " << getSubPath() << endl;
  os << "S_USR    = " << getSubUser() << endl;
  os << "EXEC     = " << getExecutable() << endl;
  os << "ARGS     = " << getArguments() << endl;
  os << "STDIN    = " << getStdin() << endl;
  os << "STDOUT   = " << getStdout() << endl;
  os << "STDERR   = " << getStderr() << endl;
  os << "E_HOST   = " << getExeHost() << endl;
  os << "E_PATH   = " << getExePath() << endl;
  os << "E_USR    = " << getExeUser() << endl;
  os << "LOG      = " << getLog() << endl;
  os << "RET_CODE = " << getRetCode() << endl;
  time = getSubTime();       os << "T_SUB    = " << str_time(&time) << endl;
  time = getStartTime();     os << "T_START  = " << str_time(&time) << endl;
  time = abs(getStopTime()); os << "T_STOP   = " << str_time(&time) << endl;
  os << "T_STAT   = " << getStatTime() << endl;
//    BossJobData::const_iterator p;
//    for (p=generalData_.begin(); p!=generalData_.end(); p++) {
//      os << " " << generalData_[p].name() << " = " 
//         << generalData_[p].value() << endl;
//     }
}

void BossJob::dumpSpecific(ostream& os) const{
  BossJobData::const_iterator p;
  for (p=specificData_.begin(); p!=specificData_.end(); p++) {
    os << " " << specificData_[p].name() << " = " 
	 << specificData_[p].value() << endl;
  }
}

void BossJob::printGeneral(string output_type, string state) const{
  if        ( output_type == "header_normal" ) {
    cout << wj("ID", 5);
    cout << wj(" ",1);
    cout << wj("S_USR",10);
    cout << wj("EXECUTABLE", 19);
    cout << wj(" ",1);
    cout << wj("ST", 3);
    cout << wj("EXE_HOST", 10);
    cout << wj(" ",1);
    cout << wj("START TIME", 15);
    cout << wj("STOP TIME", 15);     
  } else if ( output_type == "normal" ) {
    cout << wj(OSUtils::convert2string(getId()), 5);
    cout << wj(" ",1);
    cout << wj(getSubUser(), 10);
    if ( getArguments() == "" )
      cout << wj(getExecutable(), 19, "right");
    else
      cout << wj(getExecutable() + " " + getArguments(), 19, "right");
    cout << wj(" ",1);
    cout << wj(state, 3);
    cout << wj(getExeHost(), 10);
    cout << wj(" ",1);
    time_t start = getStartTime();
    time_t stop = abs(getStopTime());
    if ( start !=  0  ) 
      cout << wj(str_time(&start), 15);
    else
      cout << wj("--------------", 15);
    if ( stop !=  0  ) 
      cout << wj(str_time(&stop), 15);
    else
      cout << wj("--------------", 15);
  }
}

void BossJob::printSpecific(string output_type) const{
  BossJobData::const_iterator p;
  for ( p = specificData_.begin() ; p != specificData_.end(); p++ ) 
    if        ( output_type == "header_normal" ) {
      cout << wj(specificData_[p].name(),12);  
    } else if ( output_type == "normal" ) {
      cout << wj(specificData_[p].value(),12);  
    } else if ( output_type == "debug" ) {
      cout << "ident:type=value " 
	   << specificData_[p].name() << ":" 
	   << specificData_[p].type() << "=" 
	   << specificData_[p].value();
    }
}

string BossJob::wj(string data, unsigned int width, string justify) const {

  string str;

  unsigned int len = data.length(); 

  if ( len < width ) {
    unsigned int w = width - len;
    if ( justify == "left" ) {
      str = data + string(w,' ');
    }else if ( justify == "right" )
      str = string(w,' ') + data;
    else 
      str = data + string(w,' ');
  } else {
    if ( justify == "right" ) {
       str = data.substr(len-width,width);
    }else if ( justify == "left" )
      str = data.substr(0,width);
    else 
      str = data.substr(0,width);
  }
  // cout << "[" << len << "," << str.length() << "]";
  return str;
}

string BossJob::str_time(const time_t* time) const {
  // HH:MM:SS GG/MM
  struct tm* tt = localtime(time); 
  string hh = (tt->tm_hour>9) ? OSUtils::convert2string(tt->tm_hour):string("0")+OSUtils::convert2string(tt->tm_hour);
  string mm = (tt->tm_min>9)  ? OSUtils::convert2string(tt->tm_min) :string("0")+OSUtils::convert2string(tt->tm_min);
  string ss = (tt->tm_sec>9)  ? OSUtils::convert2string(tt->tm_sec) :string("0")+OSUtils::convert2string(tt->tm_sec);
  string gg = (tt->tm_mday>9) ? OSUtils::convert2string(tt->tm_mday):string("0")+OSUtils::convert2string(tt->tm_mday);
  string MM = ((tt->tm_mon)+1>9)  ? OSUtils::convert2string((tt->tm_mon)+1) :string("0")+OSUtils::convert2string((tt->tm_mon)+1);
  return hh+":"+mm+":"+ss+" "+gg+"/"+MM;
}

bool BossJob::existColumn(string table, string name) const {
  bool ret_val = false;
  if ( table=="JOB" )
    ret_val = generalData_.existColumn(table,name);
  else
    ret_val = specificData_.existColumn(table,name);
  return ret_val;
}

vector<string> BossJob::whichTables(string name) const {
  vector<string> ret_val;
  vector<string> types = getJobTypes();
  for(vector<string>::const_iterator ti=types.begin(); ti<types.end(); ti++) {
    if ( (*ti) != "stdjob" && existColumn((*ti),name) )
      ret_val.push_back(*ti);
}
  return ret_val;

}
