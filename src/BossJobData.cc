// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossJobData.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////


#include "BossJobData.h"

#include <sstream>

#include "BossOperatingSystem.h"

using namespace std;

BossJobData::BossJobData(){}

BossJobData::BossJobData(string name, istream& str){ add(name, str); }

BossJobData::~BossJobData(){}

void BossJobData::add(const string table, const BossJobElement& e) { 
  data_[e.name()] = e;
  string schelem = ","+e.name()+":"+e.type();
  schema_iterator psch = schema_.find(table);
  if (psch==schema_.end())
    schema_[table]=schelem.substr(1,schelem.size()); // drop the comma
  else 
    psch->second+=schelem;
}

void BossJobData::add(const string table, const string name, const string type) {
  BossJobElement e(name,type);
  add(table,e); 
}

void BossJobData::add(const string table, const string name, const string type, const string value) {
  BossJobElement e(name,type,value);
  add(table,e); 
}

void BossJobData::add(const string table, istream& schema)  {
  vector< pair<string,string> > sch = splitSchema(schema);
  vector< pair<string,string> >::const_iterator it;
  for ( it = sch.begin(); it!= sch.end(); it++ )
    add(table,it->first,it->second);
}

vector< pair<string,string> > BossJobData::splitSchema(istream& schema) const {
  vector< pair<string,string> > ret_val;
  BossOperatingSystem* sys=BossOperatingSystem::instance();
  while ( !schema.eof() ) {
    string buffer;
    char ch = schema.peek();
    while ( ch == '#' || ch == '\n' ) { // skip comment and empty lines
      getline(schema,buffer);
      ch = schema.peek();
    }
    if ( ch == EOF ) break; // exit if OEF
    getline(schema,buffer,':');
    sys->trim(buffer);
    string ident = buffer;
    if ( buffer == "" ) continue;
    getline(schema,buffer,',');
    sys->trim(buffer);
    string type = buffer;
    if ( buffer == "" ) continue;
    pair<string,string> schElem;
    schElem.first=ident;
    schElem.second=type;
    ret_val.push_back(schElem);
  }
  return ret_val;
}

vector< pair<string,string> > BossJobData::splitSchema(const string schema) const {
  std::stringstream sch;
  sch << schema;
  return splitSchema(sch);
}

bool BossJobData::existColumn(string table, string name) const {
  bool ret_val=false;
  schema_const_iterator psch = schema_.find(table);
  if(psch != schema_.end() ) {
    ret_val = existName(psch->second,name);
  }
  return ret_val;
}

bool BossJobData::existName(string schema, string name) const {
  bool ret_val=false;
  vector< pair<string,string> > sch = splitSchema(schema);
  vector< pair<string,string> >::const_iterator it;
  for ( it = sch.begin(); it!= sch.end(); it++ ) {
    if ( it->first == name ) {
      ret_val=true;
      break;
    }
  }
  return ret_val;
}

string BossJobData::getSchema() const {
  string return_val="";
  for (schema_const_iterator it=schema_.begin(); it!=schema_.end(); it++ )
    return_val+=it->second+",";
  return return_val.substr(0,return_val.size()-1); // drop the last comma
}
  
BossJobData::iterator BossJobData::find(string name) {
  return data_.find(name);
}

BossJobData::const_iterator BossJobData::find(string name) const {
  return data_.find(name);
}

void BossJobData::assign(const string name, const string value) {
  //  cerr << "Name :" << name << " Value: " << value << endl;
  (*this)[name].assign(value);
}

const BossJobElement& BossJobData::operator[](const string name) const {
  const_iterator i = data_.find(name);
  return (*this)[i];
}

const BossJobElement& BossJobData::operator[](const_iterator i) const {
  if ( i != data_.end())
    return (*i).second;
  else {
    cout << "Job element not found. "
	 << "Returning empty reference" << endl;
    BossJobElement* e = new BossJobElement();
    return *e;
  }
}

BossJobElement& BossJobData::operator[](const string name) {
  iterator i = data_.find(name);
  return (*this)[i];
}

BossJobElement& BossJobData::operator[](iterator i) {
  if ( i != data_.end())
    return (*i).second;
  else {
    cout << "Job element not found. "
	 << "Returning empty reference" << endl;
    BossJobElement* e = new BossJobElement();
    return *e;
  }
}

