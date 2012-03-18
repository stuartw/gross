#include "DbSpec.hh"

DbSpec::DbSpec(string myDbSpec){
  if(Log::level()>3) cout << "DbSpec::DbSpec: creating DbSpec object " <<endl;
  std::istringstream is(myDbSpec);
  clad_ = new BossClassAd::BossClassAd(is); //deleted in ~DbSpec()

  //initialise
  hostname_ = "UNDEFINED";
  username_ = "UNDEFINED";
  password_ = "UNDEFINED";
  database_ = "UNDEFINED";

  unixSocket_ = "NULL";
  port_ = 0;
  clientFlag_ = 0;   

  this->init(); //initialise data members
}
DbSpec::~DbSpec(){
  if(clad_) delete clad_;
}
void DbSpec::init(){
  //Set string variables
  clad_->BossClassAd::ClAdLookup("BOSS_DB_HOST", &hostname_);
  clad_->BossClassAd::ClAdLookup("BOSS_DB_USER", &username_);
  clad_->BossClassAd::ClAdLookup("BOSS_DB_USER_PW", &password_);
  clad_->BossClassAd::ClAdLookup("BOSS_DB_NAME", &database_);
  clad_->BossClassAd::ClAdLookup("BOSS_DB_SOCKET", &unixSocket_);

  //Add domain to hostname
  string domain("");
  clad_->BossClassAd::ClAdLookup("BOSS_DB_DOMAIN", &domain);

  //Take into account that sometimes people can put full domain into hostname
  if(!domain.empty()) {
    std::string::size_type sz = hostname_.find(".");
    if(sz == std::string::npos) hostname_+=string(".")+domain;
  }
  
  //non-string values:
  string myString;
  clad_->BossClassAd::ClAdLookup("BOSS_DB_PORT", &myString);
  port_ = atoi(myString.c_str());
  myString="";
  clad_->BossClassAd::ClAdLookup("BOSS_DB_CLIENT_FLAG", &myString);
  clientFlag_ = atol(myString.c_str());
  if(Log::level()>3) cout << "DbSpec::init() read in DB connection parameters: "
       <<this->hostname() << " " 
       <<this->username() << " " 
       <<this->password() << " "
       <<this->database() << " " 
       <<this->unixSocket() << " " 
       <<this->port() << " " 
       <<this->clientFlag() << " "<<endl;
}
