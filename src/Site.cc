#include "Site.hh"

using namespace std;

void Site::set_ContactString_chained(const vector<string> &input){m_ContactString_chained = input;}
void Site::set_CE_chained(const vector<string> &input){m_CE_chained = input;};

void Site::set_SiteType(const string &input){m_SiteType=input;}
void Site::set_PubDB_URL(const string &input){m_PubDB_URL = input;}
void Site::set_FileType(const string &input){ m_FileType=input; }	 
void Site::set_ValidationStatus(const string &input){ m_ValidationStatus=input; }
void Site::set_ContactString(const string &input){ m_ContactString=input; }	 
void Site::set_ContactProtocol(const string &input){ m_ContactProtocol=input; } 
void Site::set_CatalogueType(const string &input){ m_CatalogueType=input; }	 
void Site::set_SE(const string &input){ m_SE=input; }		 
void Site::set_CE(const string &input){ m_CE=input; }		 
void Site::set_Nevents(const string &input){ m_Nevents=input; }	 
void Site::set_RunRange(const string &input){	m_RunRange=input; }    
void Site::set_META_site(const void* input){	m_META_site=input; }    

string Site::get_FileType()const        { return m_FileType; }
string Site::get_ValidationStatus()const{ return m_ValidationStatus;} 
string Site::get_ContactProtocol()const { return m_ContactProtocol; } 
string Site::get_CatalogueType()const   { return m_CatalogueType;   }	   
string Site::get_SE()const	     { return m_SE; }
string Site::get_CE()const	     { return m_CE; }
string Site::get_Nevents()const	     { return m_Nevents; }
string Site::get_RunRange()const     { return m_RunRange; } 
string Site::get_ContactString()const{ 
  if ( m_CatalogueType == "XML" ) return "xmlcatalog_"+m_ContactString;
  if ( m_CatalogueType == "MYSQL" ||  m_CatalogueType == "MySQL" ) return "mysqlcatalog_mysql"+m_ContactString;
  return "non-XML and non-MySQL";
}

const void *Site::get_META()const{ return m_META_site; }

vector<int> Site::get_runs()const{
  string::size_type hyphen = m_RunRange.find("-");
  string first = m_RunRange.substr(0,hyphen);
  string second =m_RunRange.substr( hyphen+1, m_RunRange.length() );
  char **notUsed=0;
  unsigned long begin = strtoul( first.c_str(), notUsed, 0 );
  unsigned long end = strtoul( second.c_str(), notUsed, 0 );
  vector<int> runs;
  for( unsigned long i = begin; i <= end; i++ ) {
    runs.push_back(i);
  }
  return runs;
}

string Site::getSiteType()const{return m_SiteType;}
string Site::getPubDB_URL()const{return m_PubDB_URL;}
vector<string> Site::getFileType()const{
  vector<string> theStrings;
  //  if(m_FileType=="Complete") theStrings.push_back(m_FileType);
  if(m_FileType.find("Complete")!=string::npos) theStrings.push_back(m_FileType);
  if(m_FileType=="Events"){
    const void *p = get_META();
    const Site *theAttachedMETA = static_cast<const Site*>(p);
    const Site *theEvents = this;
    theStrings.push_back(theAttachedMETA->m_FileType);
    theStrings.push_back(theEvents->m_FileType);
  }
  return theStrings;
}

vector<string> Site::getValidationStatus()const{
  vector<string> theStrings;
  if(m_FileType.find("Complete")!=string::npos) theStrings.push_back(m_ValidationStatus);
  if(m_FileType=="Events"){
    const void *p = get_META();
    const Site *theAttachedMETA = static_cast<const Site*>(p);
    const Site *theEvents = this;
    theStrings.push_back(theAttachedMETA->m_ValidationStatus);
    theStrings.push_back(theEvents->m_ValidationStatus);
  }
  return theStrings;
}

vector<string> Site::getContactString()const{
  if(m_ContactString_chained.size()!=0){
    return m_ContactString_chained;
  }
  vector<string> theStrings;
  if(m_FileType.find("Complete")!=string::npos) theStrings.push_back(get_ContactString());
  if(m_FileType=="Events"){
    const void *p = get_META();
    const Site *theAttachedMETA = static_cast<const Site*>(p);
    const Site *theEvents = this;
    theStrings.push_back(theAttachedMETA->get_ContactString());
    theStrings.push_back(theEvents->get_ContactString());
  }
  return theStrings;
}

vector<string> Site::getContactProtocol()const{
  vector<string> theStrings;
  if(m_FileType.find("Complete")!=string::npos) theStrings.push_back(m_ContactProtocol);
  if(m_FileType=="Events"){
    const void *p = get_META();
    const Site *theAttachedMETA = static_cast<const Site*>(p);
    const Site *theEvents = this;
    theStrings.push_back(theAttachedMETA->m_ContactProtocol);
    theStrings.push_back(theEvents->m_ContactProtocol);
  }
  return theStrings;
}

vector<string> Site::getCatalogueType()const{
  vector<string> theStrings;
  if(m_FileType.find("Complete")!=string::npos) theStrings.push_back(m_CatalogueType);
  if(m_FileType=="Events"){
    const void *p = get_META();
    const Site *theAttachedMETA = static_cast<const Site*>(p);
    const Site *theEvents = this;
    theStrings.push_back(theAttachedMETA->m_CatalogueType);
    theStrings.push_back(theEvents->m_CatalogueType);
  }
  return theStrings;
}
vector<string> Site::getSE()const{
  vector<string> theStrings;
  if(m_FileType.find("Complete")!=string::npos) theStrings.push_back(m_SE);
  if(m_FileType=="Events"){
    const void *p = get_META();
    const Site *theAttachedMETA = static_cast<const Site*>(p);
    const Site *theEvents = this;
    theStrings.push_back(theAttachedMETA->m_SE);
    theStrings.push_back(theEvents->m_SE);
  }
  return theStrings;
}

vector<string> Site::getCE()const{
  if(m_CE_chained.size()!=0)
    return m_CE_chained;
  vector<string> theStrings;
  if(m_FileType.find("Complete")!=string::npos) theStrings.push_back(m_CE);
  if(m_FileType=="Events"){
    const void *p = get_META();
    const Site *theAttachedMETA = static_cast<const Site*>(p);
    const Site *theEvents = this;
    theStrings.push_back(theAttachedMETA->m_CE);
    theStrings.push_back(theEvents->m_CE);
  }
  return theStrings;
}
vector<string> Site::getNevents()const{
  vector<string> theStrings;
  if(m_FileType.find("Complete")!=string::npos) theStrings.push_back(m_Nevents);
  if(m_FileType=="Events"){
    const void *p = get_META();
    const Site *theAttachedMETA = static_cast<const Site*>(p);
    const Site *theEvents = this;
    theStrings.push_back(theAttachedMETA->m_Nevents);
    theStrings.push_back(theEvents->m_Nevents);
  }
  return theStrings;
}
vector<string> Site::getRunRange()const{
  vector<string> theStrings;
  if(m_FileType.find("Complete")!=string::npos) theStrings.push_back(m_RunRange);
  if(m_FileType=="Events"){
    const void *p = get_META();
    const Site *theAttachedMETA = static_cast<const Site*>(p);
    const Site *theEvents = this;
    theStrings.push_back(theAttachedMETA->m_RunRange);
    theStrings.push_back(theEvents->m_RunRange);
  }
  return theStrings;
}

vector<int> Site::getRuns()const{
  vector<int> runs;
  if(m_FileType.find("Complete")!=string::npos){
    string::size_type hyphen = m_RunRange.find("-");
    string first = m_RunRange.substr(0,hyphen);
    string second =m_RunRange.substr( hyphen+1, m_RunRange.length() );
    char **notUsed=0;
    unsigned long begin = strtoul( first.c_str(), notUsed, 0 );
    unsigned long end = strtoul( second.c_str(), notUsed, 0 );
    for( unsigned long i = begin; i <= end; i++ ) {
      runs.push_back(i);
    }
  }
  if(m_FileType=="Events"){
    const void *p = get_META();
    const Site *theAttachedMETA = static_cast<const Site*>(p);
    const Site *theEvents = this;
    vector<int> metaRuns = theAttachedMETA->get_runs();
    vector<int> eventRuns = theEvents->get_runs();
    unsigned long begin = metaRuns.front()>eventRuns.front()?metaRuns.front():eventRuns.front();
    unsigned long end = metaRuns.back()<eventRuns.back()?metaRuns.back():eventRuns.back();
    for( unsigned long i = begin; i <= end; i++ ) {
      runs.push_back(i);
    }
  }
  return runs;
}
