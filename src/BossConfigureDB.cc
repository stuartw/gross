// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 1.0
// File:    BossConfigureDB.cc
// Authors: Claudio Grandi (INFN BO)
// Date:    28/11/2003
// Note:
// /////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>
#include "BossConfigureDB.h"
//#include "BossDatabase.h"
#include "BossConfiguration.h"
#include "BossOperatingSystem.h"

using namespace std;

BossConfigureDB::BossConfigureDB() : BossCommand() {
}

BossConfigureDB::~BossConfigureDB() {}

void BossConfigureDB::printUsage() const
{
  cout << "Usage:" << endl
       << "boss ConfigureDB "
       << endl;
}

int BossConfigureDB::execute() {
  // this is just a temporary solution...
  BossOperatingSystem* sys=BossOperatingSystem::instance();
  BossConfiguration* config=BossConfiguration::instance();

  ofstream f("MySQLconfig.sql");
  if (!f) {
    cerr << "unable to write configuration file" << endl;
    return -1;
  }
  f << "USE mysql" << endl;
  // Cleanup of previous users
  f << "DELETE FROM user WHERE user = '" << config->boss_db_user() << "';" << endl;
  f << "DELETE FROM db WHERE user = '" << config->boss_db_user() << "';" << endl;
  f << "DELETE FROM tables_priv WHERE user = '" << config->boss_db_user() << "';" << endl;
  f << "DELETE FROM columns_priv WHERE user = '" << config->boss_db_user() << "';" << endl;
  f << "DELETE FROM user WHERE user = '" << config->boss_db_guest() << "';" << endl;
  f << "DELETE FROM db WHERE user = '" << config->boss_db_guest() << "';" << endl;
  f << "DELETE FROM tables_priv WHERE user = '" << config->boss_db_guest() << "';" << endl;
  f << "DELETE FROM columns_priv WHERE user = '" << config->boss_db_guest() << "';" << endl;
  // Build new users privileges
  string userpw=config->boss_db_user_pw();
  if(userpw=="NULL")
    userpw="";
  string guestpw=config->boss_db_guest_pw();
  if(guestpw=="NULL")
    guestpw="";
  f << "GRANT SELECT,INSERT,UPDATE,DELETE,CREATE,DROP ON " << config->boss_db_name() << ".* TO '" << config->boss_db_user() << "'@localhost IDENTIFIED BY '" << userpw << "' WITH GRANT OPTION;" << endl;
  f << "GRANT SELECT,INSERT,UPDATE,DELETE,CREATE,DROP ON " << config->boss_db_name() << ".* TO '" << config->boss_db_user() << "'@'" << config->boss_db_host() << "' IDENTIFIED BY '" << userpw << "' WITH GRANT OPTION;" << endl;
  f << "GRANT SELECT ON " << config->boss_db_name() << ".* TO '" << config->boss_db_guest() << "'@localhost IDENTIFIED BY '" << guestpw << "';" << endl;
  f << "GRANT SELECT ON " << config->boss_db_name() << ".* TO '" << config->boss_db_guest() << "'@'" << config->boss_db_host() << "' IDENTIFIED BY '" << guestpw << "';" << endl;
  if (strlen(config->boss_db_domain())>0) {
    f << "GRANT SELECT,INSERT,UPDATE,DELETE,CREATE,DROP ON " << config->boss_db_name() <<".* TO '" << config->boss_db_user() <<"'@'%." << config->boss_db_domain() << "' IDENTIFIED BY '" << userpw << "' WITH GRANT OPTION;" << endl;
    f << "GRANT SELECT ON " << config->boss_db_name() << ".* TO '" << config->boss_db_guest() << "'@'%." << config->boss_db_domain() << "' IDENTIFIED BY '" << guestpw << "';" << endl;
  }

  // Create database 
  string tabtyp = "";
  if ((config->boss_table_type()).size()>0)
    tabtyp = string("TYPE ") + config->boss_table_type();
  f << "CREATE DATABASE IF NOT EXISTS " << config->boss_db_name() << ";" << endl;
  f << "USE " << config->boss_db_name() << endl;

  // SCHEDULER table
  f << "CREATE TABLE IF NOT EXISTS SCHEDULER (" << endl;
  f << "NAME 		CHAR(30) BINARY NOT NULL PRIMARY KEY, " << endl;
  f << "DEF 		CHAR(1) NOT NULL DEFAULT \"N\", " << endl;
  f << "TOP_WORK_DIR 	VARCHAR(100) BINARY NOT NULL DEFAULT \"NONE\"," << endl;
  f << "SUBMIT_BIN	MEDIUMBLOB NOT NULL DEFAULT \"\"," << endl;
  f << "KILL_BIN	MEDIUMBLOB NOT NULL DEFAULT \"\"," << endl;
  f << "QUERY_BIN	MEDIUMBLOB NOT NULL DEFAULT \"\"" << endl;
  f << ")" << tabtyp << ";" << endl;
  // Insert fork scheduler

    
  // JOBTYPE table
  f << "CREATE TABLE IF NOT EXISTS JOBTYPE (" << endl;
  f << "NAME 		CHAR(30) BINARY NOT NULL PRIMARY KEY, " << endl;
  f << "SCHEMA 		BLOB NOT NULL DEFAULT \"\", " << endl;
  f << "COMMENT 	VARCHAR(100) NOT NULL DEFAULT \"\"," << endl;
  f << "PRE_BIN		MEDIUMBLOB NOT NULL DEFAULT \"\"," << endl;
  f << "RUN_BIN		MEDIUMBLOB NOT NULL DEFAULT \"\"," << endl;
  f << "POST_BIN	MEDIUMBLOB NOT NULL DEFAULT \"\"" << endl;
  f << ")" << tabtyp << ";" << endl;
  f << "INSERT INTO JOBTYPE VALUES (\"stdjob\",\"\",\"Default standard job\",\"\",\"\",\"\");" << endl;
  cout << "No schedulers are automatically registered during installation" << endl;
  cout << "Use the boss registerScheduler and the fork scripts you can find in " << config->boss_top_dir() << "/sched_examples/" << endl << endl;

  // JOB table
  f << "CREATE TABLE IF NOT EXISTS JOB (" << endl;
  f << "ID          INT AUTO_INCREMENT NOT NULL PRIMARY KEY," << endl;
  f << "TYPE        VARCHAR(30) BINARY NOT NULL DEFAULT \"\"," << endl;
  f << "SCH         VARCHAR(30) BINARY NOT NULL DEFAULT \"\"," << endl;
  f << "SID         VARCHAR(100) BINARY NOT NULL DEFAULT \"\"," << endl;
  f << "S_HOST      VARCHAR(30) BINARY NOT NULL DEFAULT \"\"," << endl;
  f << "S_PATH      VARCHAR(255) BINARY NOT NULL DEFAULT \"\"," << endl;
  f << "S_USR       VARCHAR(30) BINARY NOT NULL DEFAULT \"\"," << endl;
  f << "EXEC        VARCHAR(255) BINARY NOT NULL DEFAULT \"\"," << endl;
  f << "ARGS        VARCHAR(255) BINARY NOT NULL DEFAULT \"\"," << endl;
  f << "STDIN       VARCHAR(255) BINARY NOT NULL DEFAULT \"\"," << endl;
  f << "STDOUT      VARCHAR(255) BINARY NOT NULL DEFAULT \"\"," << endl;
  f << "STDERR      VARCHAR(255) BINARY NOT NULL DEFAULT \"\"," << endl;
  f << "E_HOST      VARCHAR(30) BINARY NOT NULL DEFAULT \"\"," << endl;
  f << "E_PATH      VARCHAR(255) BINARY NOT NULL DEFAULT \"\"," << endl;
  f << "E_USR       VARCHAR(30) BINARY NOT NULL DEFAULT \"\"," << endl;
  f << "LOG         VARCHAR(255) BINARY NOT NULL DEFAULT \"\"," << endl;
  f << "T_SUB       INT NOT NULL DEFAULT 0," << endl;
  f << "T_START     INT NOT NULL DEFAULT 0," << endl;
  f << "T_STOP      INT NOT NULL DEFAULT 0," << endl;
  f << "T_STAT      VARCHAR(50) BINARY NOT NULL DEFAULT \"\"," << endl;
  f << "RET_CODE    VARCHAR(11) BINARY NOT NULL DEFAULT \"\"" << endl;
  f << ")" << tabtyp << ";" << endl;

  //GROSS tables (temporarily put in here - maybe better in its own class, BossConfigureGrossDB??)
  
  //Analy_Task
  f<< "CREATE TABLE IF NOT EXISTS Analy_Task (" << endl;
  f<< "ID INT AUTO_INCREMENT NOT NULL PRIMARY KEY," <<endl;
  f<< "UserSpec MEDIUMBLOB NOT NULL DEFAULT \"\"," <<endl;
  f<< "JDLRem MEDIUMBLOB NOT NULL DEFAULT \"\"" <<endl;
  f<< ");" <<endl;
  
  //Analy_Job
  f<< "CREATE TABLE IF NOT EXISTS Analy_Job (" <<endl;
  f<< "ID INT AUTO_INCREMENT NOT NULL PRIMARY KEY,"<<endl;
  f<< "TaskID INT,"<<endl;
  f<< "JobID INT,"<<endl;
  f<< "BossID INT NOT NULL DEFAULT 0,"<<endl;
  f<< "DataSelect VARCHAR(255),"<<endl;
  f<< "ExecName VARCHAR(255),"<<endl;
  f<< "StdOut VARCHAR(255),"<<endl;
  f<< "StdErr VARCHAR(255),"<<endl;
  f<< "Suffix VARCHAR(255),"<<endl;
  f<< "XMLFrag MEDIUMBLOB DEFAULT \"\","<<endl;
  f<< "SboxDir VARCHAR(255),"<<endl;
  f<< "MetaFile VARCHAR(255)"<<endl;
  f<< ");"<<endl;

  //Analy_InGUIDs
  f<< "CREATE TABLE IF NOT EXISTS Analy_InGUIDs ("<<endl;
  f<< "ID INT AUTO_INCREMENT NOT NULL PRIMARY KEY,"<<endl;
  f<< "TaskID INT,"<<endl;
  f<< "JobID INT,"<<endl;
  f<< "Name VARCHAR(255)"<<endl;
  f<< ");"<<endl;
  
  //Analy_OutGUIDs
  f<< "CREATE TABLE IF NOT EXISTS Analy_OutGUIDs ("<<endl;
  f<< "ID INT AUTO_INCREMENT NOT NULL PRIMARY KEY,"<<endl;
  f<< "TaskID INT,"<<endl;
  f<< "JobID INT,"<<endl;
  f<< "Name VARCHAR(255)"<<endl;
  f<< ");"<<endl;

  //Analy_LocalIn
  f<< "CREATE TABLE IF NOT EXISTS Analy_LocalIn ("<<endl;
  f<< "ID INT AUTO_INCREMENT NOT NULL PRIMARY KEY,"<<endl;
  f<< "TaskID INT,"<<endl;
  f<< "JobID INT,"<<endl;
  f<< "Name VARCHAR(255)"<<endl;
  f<< ");"<<endl;

  //Analy_OutSandbox
  f<< "CREATE TABLE IF NOT EXISTS Analy_OutSandbox ("<<endl;
  f<< "ID INT AUTO_INCREMENT NOT NULL PRIMARY KEY,"<<endl;
  f<< "TaskID INT,"<<endl;
  f<< "JobID INT,"<<endl;
  f<< "Name VARCHAR(255)"<<endl;
  f<< ");"<<endl;

  //Analy_Wrapper
  f<< "CREATE TABLE IF NOT EXISTS Analy_Wrapper ("<<endl;
  f<< "name VARCHAR(255) NOT NULL PRIMARY KEY,"<<endl;
  f<< "script MEDIUMBLOB NOT NULL DEFAULT \"\""<<endl;
  f<< ");"<<endl;


  // flush privileges
  f << "FLUSH PRIVILEGES;" << endl;

  f.close();

  // Grid configuration file (allow connections from outside the domain)
  ofstream fg("MySQLGridConfig.sql");
  if (!fg) {
    cerr << "unable to write configuration file" << endl;
    return -2;
  }
  fg << "USE mysql" << endl;
  fg << "GRANT SELECT,INSERT,UPDATE,DELETE,CREATE,DROP ON " << config->boss_db_name() << ".* TO '" << config->boss_db_user() << "'@'%' IDENTIFIED BY '" << userpw << "' WITH GRANT OPTION;" << endl;
  fg << "GRANT SELECT ON " << config->boss_db_name() << ".* TO '" << config->boss_db_guest() << "'@'%' IDENTIFIED BY '" << guestpw << "';" << endl;
  fg << "FLUSH PRIVILEGES;" << endl;

  fg.close();

   string connect_string;
   connect_string = "-u root -s -f";
   if (string(config->boss_db_host()) != sys->getHostName())
     connect_string += " -h " + string(config->boss_db_host());
   if (config->boss_db_port() != 0)
     connect_string += " -P " + sys->convert2string(config->boss_db_port());
   if (string(config->boss_db_socket()) != "NULL" && string(config->boss_db_socket()) !="" )
     connect_string += " -S " + string(config->boss_db_socket());

   cout << "Ask your MySQL administrator to execute the command:" << endl;
   cout << "mysql " << connect_string << " [-p] < MySQLconfig.sql" << endl << endl;
   cout << "If you need to grant access to the MySQL server from any host (e.g. using" << endl;
   cout << "a grid scheduler) ask your MySQL administrator to execute the command:" << endl;
   cout << "mysql " << connect_string << " [-p] < MySQLGridConfig.sql" << endl;
  
  return 0;
}









