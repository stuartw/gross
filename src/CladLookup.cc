#include "CladLookup.hh"

CladLookup::CladLookup(const string& myCladSpec) : jdlDump_("NULL"){
  if(Log::level()>2) cout << "CladLookup::CladLookup() Creating CladLookup object " <<endl;
  std::istringstream is(myCladSpec);
  //clad_ = new BossClassAd::BossClassAd(is);
  clad_ = new CAL::ClassAdLite();
  CAL::read(*clad_,is);
}
CladLookup::~CladLookup(){
  if(clad_) delete clad_;
}
const string CladLookup::jdlDump() const {
  if(jdlDump_!="NULL") return jdlDump_; //This to return a forced jdlDump value set in setJdlDump
  ostringstream os;
  //for(BossClassAd::const_iterator i = clad_->begin() ; i != clad_->end() ; i++) { 
  for(CAL::ClassAdLite::const_iterator i = clad_->begin(); i != clad_->end(); i++) {
    //string myIdent=clad_->getIdent(i);
    string myIdent=i->first;
    string myValue=i->second;
    //getIdent and getValue (in)conveniently remove brackets and 
    //other syntax needed again in jdl, so we need to put them back in - ug.
    string::size_type i = myValue.find_first_not_of(" \t");
    if (! (cladMap_.find(myIdent) == cladMap_.end())) continue;
 //Do we really need this? TO DO TEST !!!
    if (myIdent!="Requirements") {
      //string::size_type i = myValue.find_first_not_of(" \t");
      if(!(myValue.substr(i,1)=="{" || myValue.substr(i,1)=="\"" || myValue.substr(i,1)=="(")) {
        myValue.insert(i,"\"");
        string::size_type j = myValue.find_last_not_of(" \t");
        myValue.insert(j+1,"\"");
      }
      i = myValue.find_last_not_of(" \t");
    }
    //else {
    //  
    //}
      //myValue.insert(i+1, ";");
    os << myIdent << " = " << myValue << ";" << endl;
  }  
  return os.str();
}
void CladLookup::jdlDump(const string myJdlDump) {
  //This forces jdlDump value to be set
  jdlDump_ = myJdlDump;
}

//This method should really be in the classad reader object.
void CladLookup::string2vec (const string& line, vector<string>& strings) const {
  string::size_type i1, i2;

  i1=line.find("\"");
  if(i1==string::npos) {
    strings.push_back(line);   //case where line contains a single, non-quoted string
  }
  while (i1 != string::npos) {
    i1++;
    i2 = line.find("\"",i1);
    strings.push_back(line.substr(i1,(i2-i1))); //case where line contains multiple quoted strings
    i1 = line.find("\"",i2+1);
  }
}
const string CladLookup::lookup(const string& myKey) {
  if(cladMap_.find(myKey) == cladMap_.end()) {
    string buff("");
    //clad_->BossClassAd::ClAdLookup(myKey, &buff);
    CAL::lookup(*clad_,myKey,buff);
    CAL::removeOuterQuotes(buff);
    cladMap_[myKey] = buff;
  }
  return cladMap_.find(myKey)->second;
}
const string CladLookup::read(const string& myKey) {
  return lookup(myKey);
}
int CladLookup::read(const string myKey, vector<string>& myVec) {
  const string line = lookup(myKey);
  if(line.empty()) return EXIT_FAILURE;

  string2vec(line, myVec);
  return EXIT_SUCCESS;
}
ostream& operator<<(ostream& os, const CladLookup& myT) {
  os<<myT.jdlDump();
  return os;
}
