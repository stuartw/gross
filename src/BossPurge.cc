// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossQuery.cc
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <iostream>
#include <ctime>
#include "BossPurge.h"
#include "BossJob.h"
#include "BossDatabase.h"
#include "BossScheduler.h"

using namespace std;
 
BossPurge::BossPurge() : BossCommand() {
  opt_["-before"] = ""; 
  opt_["-noprompt"] = "FALSE"; 
}

BossPurge::~BossPurge() {}

void BossPurge::printUsage() const
{
  cout << "Usage:" << endl
       << "boss purge " << endl;
  cout << "           -before <date mm/dd/yyyy> " << endl
       << "           -noprompt " << endl
       << endl;
}

int BossPurge::execute() {
  //  for (Options_const_iterator i=opt_.begin();i!=opt_.end();i++)
  //    cout << i->first << "=" << i->second << endl;
  
  BossDatabase db("super");  

  int mm,dd,yy;
  string date = opt_["-before"];
  // check the date
  if ( chkDate(date,mm,dd,yy) ) {
    cout << "Wrong date" << endl;
    return -1;
  }
  // DEBUG
  // cout << mm << " " << dd << " " << yy << endl;
  // END DEBUG

  vector<BossJob*> jobs = db.jobs('a',string(""));
  
  cout << "Check " << jobs.size() << " jobs" << endl;

  string state;
  struct tm* job_time;
  time_t tt;
  int found = 0;
  vector<BossJob*>::iterator i=jobs.begin();
  BossScheduler sched(&db);
  while ( i != jobs.end() ) {
    state = sched.status(*i);
    tt = (*i)->getSubTime();
    job_time = localtime(&tt);
    if ( lessDate(job_time->tm_mon+1,job_time->tm_mday,(job_time->tm_year)+1900,
		  mm,dd,yy) && state != "R" && state != "I" ) {
      cout << "Job ID " << (*i)->getId() << " match" << endl;
      (*i)->printGeneral("normal",state);
      cout << endl;
      if ( opt_["-noprompt"] == "TRUE" ) {
	found++;
	if ( db.deleteJob((*i)->getId()) ) {
	  cout << "an error occured while removing job, try later" << endl;
	  // at this point do not return, trying to eliminate db inconsistency
	}
	vector<string> types = (*i)->getJobTypes();
	for(vector<string>::const_iterator ti=types.begin(); ti<types.end(); ti++) {
	  if ( *ti != "stdjob" ) {
	    if ( db.deleteSpecificJob((*ti),(*i)->getId()) ) {
	      cout << "an error occured while removing job specific data, try later" << endl;
	      // at this point do not return, trying to eliminate db inconsistency
	    }
	  }
	}
      } else {
	if ( prompt() ) {
	  found++;
	  if ( db.deleteJob((*i)->getId()) ) {
	    cout << "an error occured while removing job, try later" << endl;
	    // at this point do not return, trying to eliminate db inconsistency
	  }
	  vector<string> types = (*i)->getJobTypes();
	  for(vector<string>::const_iterator ti=types.begin(); ti<types.end(); ti++) {
	    if ( *ti != "stdjob" ) {
	      if ( db.deleteSpecificJob((*ti),(*i)->getId()) ) {
		cout << "an error occured while removing job specific data, try later" << endl;
		// at this point do not return, trying to eliminate db inconsistency
	      }
	    }
	  }
	}
      }
    }
    i++; 
    // i = jobs.erase(i);
  } 
    cout << "Delete " << found << " jobs not running before date " 
	 << mm << " " << dd << " " << yy
	 << endl;
  
  return 0;
}

// 1 iff the user answer y
// 0 otherwise
int BossPurge::prompt() {

  string answer;
  cout << "delete job ?";
  do {
    cout << " y/n ? ";
    cin >> answer;
  } while ( answer != "y" && answer != "n" );

  if ( answer == "y" ) 
    return 1;
  else
    return 0;
}

// check if date is the form mm/dd/yyyy
// return 0 if date is ok and update M, D, Y
// return -1 otherwise
int BossPurge::chkDate(string date, int& M, int& D, int& Y) {

  string mm,dd,yy;

  int s,f;
  
  s = 0;
  f = date.find("/",s);
  mm = date.substr(s, f-s);
  s = f+1;
  f = date.find("/",s);
  dd = date.substr(s, f-s);
  s = f+1;
  f = date.find("/",s);
  yy = date.substr(s, f-s);

  M = atoi(mm.c_str()),
  D = atoi(dd.c_str()),
  Y = atoi(yy.c_str());

  if ( M < 1 || M > 13 ) 
    return -1;

  if ( Y < 1970 || Y > 2100 ) 
    return -1;

  if ( D < 0 ) 
    return -1;

  if ( ( (M == 1) || (M==3) || (M==5) || (M==7) || (M==8) || (M==10) || (M==12) ) 
       && D > 31 ) 
    return -1;

  if ( ( (M == 4) || (M==6) || (M==9) || (M==11) ) 
       && D > 30 ) 
    return -1;

  if ( D > 29  && M == 2 )
    return -1;

  // ??????????
  if ( D > 28 && M == 2 && (Y % 4) != 0 )
    return -1;

  return 0;
}

// 1 iff date1 is less than date2
// 0 otherwise
int BossPurge::lessDate(int m1,int d1,int y1,int m2,int d2,int y2) {

  // DEBUG
  // cout << "Diffing date " << m1 << "/" << d1 << "/" << y1 << " with " 
  //      << m2 << "/" << d2 << "/" << y2 << endl;
  // END DEBUG
  if ( y1 < y2 ) 
    return 1;
  if ( y1 == y2 && m1 < m2 )
    return 1;
  if ( y1 == y2 && m1 == m2 && d1 < d2 )
    return 1;

  return 0;
}
