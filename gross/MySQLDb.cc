#include "MySQLDb.hh"

#include "DbSpec.hh"

MySQLDb::MySQLDb(DbSpec* myDbSpec) {
  current_connection_=0;
  dbSpec_ = myDbSpec;
}
MySQLDb::~MySQLDb() {
  disconnect();
  if(Log::level()>2) cout << "Disconnecting from database"<<endl;
}
int MySQLDb::init() {
  if(connect()) {
    cerr<<"MySQLDb::MySQLDb(): Error: Failed to establish DB connectivity"<<endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
int MySQLDb::disconnect() {  
  if(current_connection_) mysql_close(current_connection_);
  return EXIT_SUCCESS;
}
int MySQLDb::connect() {
  if(current_connection_==0) {
    current_connection_ = mysql_init(NULL);
    if(!current_connection_) {
      cerr << "MySQLDb::connect(): Error: Cannot initialise connection " 
	   << mysql_error(current_connection_)<< endl;
      return EXIT_FAILURE;
    }
    
    if(!mysql_real_connect(current_connection_, (dbSpec_->hostname()).c_str(),
			   (dbSpec_->username()).c_str(), (dbSpec_->password()).c_str(), 
			   (dbSpec_->database()).c_str(), dbSpec_->port(), 
			   (dbSpec_->unixSocket()).c_str(), dbSpec_->clientFlag()) ) {
      cerr << "MySQLDb::connect(): Error: Cannot make connection "
	   << mysql_error(current_connection_) << endl;
      disconnect();
      return EXIT_FAILURE;
    }
  }
    return EXIT_SUCCESS;
}
