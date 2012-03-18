#include <iostream>
#include <string>

#include "BossOperatingSystem.h"

int main(int argc, char** argv) {

  //               ==========================
  //                 Preliminary operations
  //               ==========================

  BossOperatingSystem* sys=BossOperatingSystem::instance();
  
  // Job identifier
//    if (argc < 2)
//      return -1;
  string nam = argv[0];
  
  cout << "This file is " << sys->basename(nam) << endl;
  cout << "It is located in " << sys->dirname(nam) << endl;

  return 0;
}
