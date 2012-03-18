// /////////////////////////////////////////////////////////////////////
// Program: dbUpdator
// Version: 1.0
// File:    dbUpdator.cpp
// Authors: Claudio Grandi (INFN BO)
// Date:    03/10/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <sstream>

#include "BossConfiguration.h"
#include "OperatingSystem.h"
#include "BossRealTimeUpdator.h"
#include "BossFileMonitor.h"

using namespace std;

int main(int argc, char** argv) {

  //               ==========================
  //                 Preliminary operations
  //               ==========================

  // Job identifier
  if (argc < 2)
    return -1;
  int id = atol(argv[1]);
  string strid = OSUtils::convert2string(id);
  //Input (journal) file
  string journalFileN = string("BossJournal_")+strid+".txt";
  if (!OSUtils::fileExist(journalFileN)) {
    cerr << "dbUpdator is unable to find journal file " << journalFileN 
	 << ". Abort!" << endl;
    return -2;
  }
  BossFileMonitor journal(journalFileN);
#ifdef LOGL3
  cerr << "dbUpdator is reading from: " << journalFileN << endl;
#endif

  //               ==========================
  //                   Start event loop
  //               ==========================
  // 
  BossRealTimeUpdator upd(id);
  BossConfiguration* config=BossConfiguration::instance();

  //BossRealTimeUpdator* upd = new BossRealTimeUpdator(id);
  bool stop_loop = false;
  time_t lastUpd = 0;
  do {
    // Minimum update interval
    OSUtils::sleep(config->boss_min_upd_int());
    std::stringstream frun;
    int newl = journal.newlines(frun, stop_loop);
    if (newl) {
      BossUpdateSet us(frun);
      time_t now = OSUtils::getTime();
      // Maximum update interval
      bool force = now-lastUpd > config->boss_max_upd_int()/2; 
      if ( us.size() > 0 || force ) {
	std::string stime = OSUtils::convert2string(now);
	us.add(BossUpdateElement(id,"JOB","T_LAST",OSUtils::convert2string(now)));
	upd.update(us);
	lastUpd = now;
      }
    }
  } while (!stop_loop);
  return 0;
}
