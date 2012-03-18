// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 1.0
// File:    BossRealTimeUpdator.h
// Authors: Claudio Grandi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_REAL_TIME_UPDATOR_H
#define BOSS_REAL_TIME_UPDATOR_H

#include "BossUpdateSet.h"
#include <string>
#include <fstream>
#include <map>
#include <iostream>

class BossRealTimeUpdator { 

private:
  int jobid_;
  typedef std::map< std::string,std::string,std::less<std::string> > Updates;

public:
  explicit BossRealTimeUpdator(int jobid=0) : jobid_(jobid) {}

  virtual ~BossRealTimeUpdator() {}

  int update(const BossUpdateSet& us) {
    //cerr << "BossRealTimeUpdator::update!!!" << endl;
    int ret_val = 0;
    //us.dump(cerr);
    Updates upd;
    for (BossUpdateSet::const_iterator i=us.begin(); i<us.end(); i++) {
      if( (*i).jobid()!=jobid_) {
	//std::cerr << "Found updates for job " << (*i).jobid() << std::endl;
	continue;
      }
      std::string id = (*i).table() + "::" + (*i).varname();
      upd[id]=(*i).varvalue();
      //#ifdef LOGL3
      //      // LOGGING
      //      std::cout << "Update " << id << " -> " << (*i).varvalue() << std::endl;
      //#endif  
      
    }
    if (connect()) {
      for ( Updates::const_iterator mi=upd.begin(); mi!= upd.end(); mi++) {
#ifdef LOGL3
	std::cout << "Update " << mi->first << " -> " << mi->second << std::endl;
#endif  
	int delim = (mi->first).find_first_of("::",0);
	if (!updateJobParameter(jobid_,
				(mi->first).substr(0,delim),
				(mi->first).substr(delim+2,(mi->first).size()-delim-2),
				mi->second) ) {
	  ret_val++;
	}
      }
      disconnect();
    } else {
      std::cerr << "BossRealTimeUpdator: error connecting... skip update." << std::endl;
    }
    return ret_val;
  }

  virtual bool connect() { return true; }

  virtual bool disconnect() { return true; }  

  virtual int updateJobParameter(int id, std::string tab, std::string nam, std::string val) {
    std::cout << "UPDATE " << tab << " SET " << nam << "=" << val 
	 << " WHERE JOBID = " << id << std::endl;
    return 1;
  }

};
#endif

