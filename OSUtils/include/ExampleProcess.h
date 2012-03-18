// /////////////////////////////////////////////////////////////////////
// Program: Utilities
// Version: 1.0
// File:    ExampleProcess.h
// Authors: Claudio Grandi (INFN BO)
// Date:    15/03/2004
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef EXAMPLE_PROCESS_H
#define EXAMPLE_PROCESS_H

#include <string>
#include <fstream>

#include "Process.h"
#include "OperatingSystem.h"

namespace OSUtils {
  class ExampleProcess : public Process {
  private:
    int waitTime_;
  public:
    ExampleProcess(int wt) : Process("Example"), waitTime_(wt) {}
    
    int start() {
      // use cerr because stderr is unbuffered (stdout has a ~8K buffer)
      std::cerr << "ExampleProcess starts sleeping for " << waitTime_ << " seconds..." << std::endl;
      OSUtils::sleep(waitTime_);
      std::cerr << "ExampleProcess: Done!" << std::endl;
      return 0;
    }
    ~ExampleProcess() {}
  };
}
#endif
