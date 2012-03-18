#include "DbSpec.hh"

DbSpec::DbSpec(string myDbSpec){
  if(Log::level()>3) cout << "DbSpec::DbSpec: creating DbSpec object " <<endl;
  //std::istringstream is(myDbSpec);
  //clad_ = new BossClassAd::BossClassAd(is); //deleted in ~DbSpec()
  clad_ = new CladLookup(myDbSpec);

  //initialise
  hostname_ = "UNDEFINED";
  username_ = "UNDEFINED";
  password_ = "UNDEFINED";
  database_ = "UNDEFINED";

  unixSocket_ = "NULL";
  port_ = 0;
  clientFlag_ = 0;   

  init(); //initialise data members
}
DbSpec::~DbSpec(){
  if(clad_) delete clad_;
}
void DbSpec::init(){
  //Set string variables
  //clad_->BossClassAd::ClAdLookup("BOSS_DB_HOST", &hostname_);
  hostname_ = clad_->read("DB_HOST");
  //clad_->BossClassAd::ClAdLookup("BOSS_DB_USER", &username_);
  username_ = clad_->read("DB_USER");
  //clad_->BossClassAd::ClAdLookup("BOSS_DB_USER_PW", &password_);
  password_ = clad_->read("DB_USER_PW");
  //clad_->BossClassAd::ClAdLookup("BOSS_DB_NAME", &database_);
  database_ = clad_->read("DB_NAME");
  //clad_->BossClassAd::ClAdLookup("BOSS_DB_SOCKET", &unixSocket_);
  unixSocket_ = clad_->read("DB_SOCKET");
  
  //Add domain to hostname
  string domain("");
  //clad_->BossClassAd::ClAdLookup("BOSS_DB_DOMAIN", &domain);
  domain = clad_->read("DB_DOMAIN");

  //Take into account that sometimes people can put full domain into hostname
  if(!domain.empty()) {
    std::string::size_type sz = hostname_.find(".");
    if(sz == std::string::npos) hostname_+=string(".")+domain;
  }
  
  //non-string values:
  string myString;
  //clad_->BossClassAd::ClAdLookup("BOSS_DB_PORT", &myString);
  myString = clad_->read("DB_PORT");
  port_ = atoi(myString.c_str());
  myString="";
  //clad_->BossClassAd::ClAdLookup("BOSS_DB_CLIENT_FLAG", &myString);
  myString = clad_->read("DB_CLIENT_FLAG");
  clientFlag_ = atol(myString.c_str());
  if(Log::level()>3) cout << "DbSpec::init() read in DB connection parameters: "
			  <<hostname() << " " 
			  <<username() << " " 
			  <<password() << " "
			  <<database() << " " 
			  <<unixSocket() << " " 
			  <<port() << " " 
			  <<clientFlag() << " "<<endl;
}
