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
#include "BossOperatingSystem.h"
#include "BossRealTimeMySQLUpdator.h"
#include "BossFileMonitor.h"

using namespace std;

int main(int argc, char** argv) {

  //               ==========================
  //                 Preliminary operations
  //               ==========================

  BossOperatingSystem* sys=BossOperatingSystem::instance();
  
  // Job identifier
  if (argc < 2)
    return -1;
  int id = atol(argv[1]);
  string strid = sys->convert2string(id);
  //Input (journal) file
  string journalFileN = string("BossJournal_")+strid+".txt";
  if (!sys->fileExist(journalFileN)) {
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
  BossRealTimeUpdator* upd = new BossRealTimeMySQLUpdator(id);
  BossConfiguration* config=BossConfiguration::instance();

  //BossRealTimeUpdator* upd = new BossRealTimeUpdator(id);
  bool stop_loop = false;
  do {
    sys->sleep(config->boss_upd_interval());// Update interval
    std::stringstream frun;
    int newl = journal.newlines(frun, stop_loop);
    if (newl) {
      BossUpdateSet us(frun);
      if ( us.size() > 0 ) {
	//us.dump(cerr);
	upd->update(us);
      } 
    }
  } while (!stop_loop);
  delete upd;
  // testtesttest
  // sys->sleep(10*config->boss_upd_interval());// Update interval
  return 0;
}
