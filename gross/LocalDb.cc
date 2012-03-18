#include "LocalDb.hh"

#include "DbSpec.hh"

LocalDb* LocalDb::instance_=0;
LocalDb* LocalDb::instance() {
  if(!instance_)
    instance_= new LocalDb();
  return instance_;
}

LocalDb::LocalDb() {
  current_connection_=0;
  dbSpec_ = 0;
}
LocalDb::~LocalDb() {
  disconnect();
  //  if(Log::level()>2) cout << "Disconnecting from database"<<endl;
}
int LocalDb::init(DbSpec* myDbSpec) {
  dbSpec_ = myDbSpec;
  if(connect()) {
    cerr<<"LocalDb::LocalDb(): Error: Failed to establish DB connectivity"<<endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
int LocalDb::disconnect() {  
  if(current_connection_) mysql_close(current_connection_);
  return EXIT_SUCCESS;
}
int LocalDb::connect() {
  if(current_connection_==0) {
    current_connection_ = mysql_init(NULL);
    if(!current_connection_) {
      cerr << "LocalDb::connect(): Error: Cannot initialise mysql object" 
	   << mysql_error(current_connection_)<< endl;
      return EXIT_FAILURE;
    }
    
    if(!mysql_real_connect(current_connection_, (dbSpec_->hostname()).c_str(),
			   (dbSpec_->username()).c_str(), (dbSpec_->password()).c_str(), 
			   (dbSpec_->database()).c_str(), dbSpec_->port(), 
			   (dbSpec_->unixSocket()).c_str(), dbSpec_->clientFlag()) ) {
      cerr << "LocalDb::connect(): Error: Cannot make connection "
	   << mysql_error(current_connection_) << endl;
      disconnect();
      return EXIT_FAILURE;
    }
  }
    return EXIT_SUCCESS;
}
int LocalDb::tableRead(string myTable, string myKey, 
		       string mySelection, vector<string>& myValues){
  ostringstream query;
  query <<"SELECT "<< myKey << " from " << myTable << " WHERE " << mySelection;

  if(Log::level()>2) cout <<"LocalDb::tableRead() SQL query is "<<query.str()<<endl;
  
  if(mysql_query(LocalDb::instance()->connection(), (query.str()).c_str())) {
    cerr << "LocalDb::tableRead(): Error: cannot query table " << myTable << " "
	 <<mysql_error(LocalDb::instance()->connection())<<endl;
    return EXIT_FAILURE;
  }

  MYSQL_RES* result;
  result = mysql_store_result(LocalDb::instance()->connection());
  if(!result) {
    cerr<<"LocalDb::tableRead() Error querying database"<<endl;
    return EXIT_FAILURE;
  }
  if(mysql_num_fields(result)!=1) {
    cerr<<"LocalDb::tableRead() Error: too many/few rows in db"<<endl;
    return EXIT_FAILURE;
  }

  for(unsigned int i=0;i<mysql_num_rows(result);i++){
    MYSQL_ROW row = mysql_fetch_row(result);
    if(row[0]) myValues.push_back(row[0]); //Result is not NULL
    else myValues.push_back("");
  }

  mysql_free_result(result);
  return EXIT_SUCCESS;
}
int LocalDb::tableSave(string myTable, string myKeys, string myValues) const {
  ostringstream query;
  query<< "INSERT INTO " << myTable << "( "<< myKeys << " )" << " VALUES ( "<< myValues <<" )";
  if(Log::level()>2) cout << "LocalDb::tableSave() SQL query is " <<query.str()<<endl;
  if(mysql_query(LocalDb::instance()->connection(), (query.str()).c_str())) {
    cerr << "LocalDb::tablesave(): Error: saving into table " << myTable <<" "
	 << mysql_error(LocalDb::instance()->connection()) << endl;
    return EXIT_FAILURE;
  }
  if(Log::level()>2) cout << "LocalDb::tablesave(): saved into table "<< myTable << " with id "
       << mysql_insert_id(LocalDb::instance()->connection()) <<endl;
  return EXIT_SUCCESS;
}
int LocalDb::tableUpdate(string myTable, string myKey, string myValue, string mySelection /*= ""*/) const {
  ostringstream query;
  query<< "UPDATE " << myTable << " SET "<< myKey << "=" << myValue;
  if(mySelection.length()) query<<" WHERE "<<mySelection;
  if(Log::level()>2) cout << "LocalDb::tableUpdate() SQL query is " <<query.str()<<endl;
  if(mysql_query(LocalDb::instance()->connection(), (query.str()).c_str())) {
    cerr << "LocalDb::tableUpdate(): Error: updating table " << myTable <<" "
	 << mysql_error(LocalDb::instance()->connection()) << endl;
    return EXIT_FAILURE;
  }
  if(Log::level()>2) cout << "LocalDb::tableUpdate(): updated table "<< myTable << " with last id "
			  << mysql_insert_id(LocalDb::instance()->connection()) <<endl;
  return EXIT_SUCCESS;
}
int LocalDb::tableDelete(string myTable, string mySelection) const {
  ostringstream query;
  query << "DELETE FROM " << myTable << " WHERE " << mySelection;
  if(Log::level()>2) cout << "LocalDb::tableDelete() SQL query is " <<query.str()<<endl;
  if(mysql_query(LocalDb::instance()->connection(), (query.str()).c_str())) {
    cerr << "LocalDb::tableDelete(): Error: deleting entries from table " 
	 << myTable << "with selection " << mySelection << " " 
	 << mysql_error(LocalDb::instance()->connection()) << endl;
    return EXIT_FAILURE;
  }
  if(Log::level()>2) cout << "LocalDb::tableDelete(): Deleted "<< 
		       mysql_affected_rows(LocalDb::instance()->connection()) <<
		       " rows from table " << myTable << " with selection " << mySelection <<endl;
  return EXIT_SUCCESS;
}
int LocalDb::tableCreate(string myTable, vector<string> myFields) const {
  ostringstream query;
  ostringstream values;
  for (vector<string>::const_iterator i=myFields.begin(); i!=myFields.end(); i++) {
    values << (*i);
    if (i<myFields.end()-1) values << ", ";
  }
  query << "CREATE TABLE IF NOT EXISTS " << myTable << " (" << values.str() << ")";
  if(Log::level()>2) cout << "LocalDb::tableCreate() SQL query is " << query.str() <<endl;
  if(mysql_query(LocalDb::instance()->connection(), (query.str()).c_str())) {
    cerr << "LocalDb::tableCreate(): Error: Creating table "
         << myTable << " with values " << values.str() << " "
	 << mysql_error(LocalDb::instance()->connection()) << endl;
    return EXIT_FAILURE;
  }
  if(Log::level()>2) cout << "LocalDb::tableCreate(): Created "<<
	                     mysql_affected_rows(LocalDb::instance()->connection()) <<
			     " table " << myTable << " with values " << values.str() <<endl;
  return EXIT_SUCCESS;
}
int LocalDb::maxCol(string myTable,string myCol) const {
  ostringstream query;
  query << "SELECT MAX(" << myCol << ") FROM " << myTable;
  if(Log::level()>2) cout << "LocalDb::maxCol() SQL query is " <<query.str()<<endl;
  if(mysql_query(LocalDb::instance()->connection(), (query.str()).c_str())) {
    cerr << "LocalDb::maxCol(): Error getting max value for table " 
	 << myTable << "column " << myCol << " " 
	 << mysql_error(LocalDb::instance()->connection()) << endl;
    return 0;
  }
  MYSQL_RES* result;
  result = mysql_store_result(LocalDb::instance()->connection());
  if(!result) {
    cerr<<"LocalDb::maxCol() Error querying database"<<endl;
    return 0;
  }
  if(mysql_num_fields(result)!=1) {
    cerr<<"LocalDb::maxCol() Error: too many/few rows in db"<<endl;
    return 0;
  }  
  MYSQL_ROW row = mysql_fetch_row(result);
  int rval;
  if(!row[0]) {rval=0;} //NULL result, eg if database is empty
  else {rval = atoi(row[0]);}
  mysql_free_result(result);  
  return rval;
}
string LocalDb::escapeString(const string& myString) const {
  int medBlobLen=16777215;
  int clen=myString.length();
  if(clen>medBlobLen) {
    cerr<<"LocalDb::escapeString string length exceeds medium blob storage in db "<<endl;
    return "";
  }
  char* cbuffer = new char[medBlobLen*2]; //Corresponds to twice Max Blob Size
  const char* cFrom = myString.c_str();
  mysql_real_escape_string(LocalDb::instance()->connection(), cbuffer, cFrom, clen);

  ostringstream os;
  os<<"'"<<cbuffer<<"'";
  delete(cbuffer);
  
  return os.str();
}
