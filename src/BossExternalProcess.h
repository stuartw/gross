// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossExternalProcess.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_EXTERNAL_PROCESS_H
#define BOSS_EXTERNAL_PROCESS_H

#include <string>
#include <vector>
#include <stdlib.h>
#include <errno.h> 
#include "BossProcess.h"
#include "BossOperatingSystem.h"

class BossExternalProcess : public BossProcess {
private:
  std::string executable_;
  std::string stdin_;
  std::string stdout_;
  std::string stderr_;
  std::vector<std::string> args_;
public:
  BossExternalProcess( std::string exec="", std::string in="",
		       std::string out="" , std::string err="",
		       std::string arg="") : BossProcess(), executable_(exec), 
		       stdin_(in), stdout_(out), stderr_(err) {
    setArgs(arg);
  }

  void setExec       (std::string exec) { executable_=exec; }

  void setInStream   (std::string in  ) { stdin_=in; }

  void setOutStream  (std::string out ) { stdout_=out; }

  void setEerrStream (std::string err ) { stderr_=err; }

  void setArgs       (std::string arg ) {
    BossOperatingSystem* sys=BossOperatingSystem::instance();
    args_ = sys->splitString(arg,' ');
  }

  int start() {
    BossOperatingSystem* sys=BossOperatingSystem::instance();
    char* argv[args_.size()+2];
    int index = 0;
    // Executable
    if ( ! sys->fileExist(executable_) ) {
      std::cerr << "Executable " << executable_ 
	   << " does not exist. Abort." << std::endl;
      return -1;
    }
    argv[index++]=const_cast<char*>(executable_.c_str());
    // Arguments
    std::vector<std::string>::const_iterator si;
    for (si=args_.begin(); si<args_.end(); si++) {
      argv[index++] = const_cast<char*>((*si).c_str());
    }
    argv[index] = NULL;
    
    // Check STDIN
    if (stdin_!="/dev/null" && stdin_!="") {
      if ( ! sys->fileExist(stdin_) ) {
	std::cerr << "Standard input " << stdin_ 
	     << " does not exist. Abort." << std::endl;
	return -2;
      }
    }
//      // Check STDOUT
//      if (stdout_!="/dev/null" && stdout_!="") {
//        if ( ! sys->fileExist(stdout_) ) {
//  	std::cerr << "Standard output " << stdout_ 
//  	     << " does not exist. Abort." << std::endl;
//  	return -3;
//        }
//      }
//      // Check STDERR
//      if (stderr_!="/dev/null" && stderr_!="") {
//        if ( ! sys->fileExist(stderr_) ) {
//  	std::cerr << "Standard error " << stderr_ 
//  	     << " does not exist. Abort." << std::endl;
//  	return -4;
//        }
//      }
#ifdef LOGL2
    std::cout << "Executing: " << argv[0] << " ";
    for (unsigned int k=1; k<=args_.size(); k++)
      std::cout << argv[k] << " ";
    std::cout << std::endl;
    std::cout << "     PID : " << sys->getPid() << std::endl;
    std::cout << "   StdIn : " << stdin_ << std::endl;
    std::cout << "   StdOut: " << stdout_ << std::endl;
    std::cout << "   StdErr: " << stderr_ << std::endl;
#endif
    // redirect STDIN/OUT/ERR
    // cerr << "redirect stdin to " << stdin_ << "...";
    if (stdin_!="") {
      if ( freopen(stdin_.c_str(),"r",stdin) == NULL ) {
	std::cerr << "Error: unable to redirect stdin" << std::endl;
	std::cerr << "System error " << strerror(errno) << std::endl;
	return -5;
      }
    }
    // cerr << "Done!" << endl;
    // cerr << "redirect stdout to " << stdout_ << "...";
    if (stdout_!="") {
      if ( freopen(stdout_.c_str(),"w",stdout) == NULL ) {
	std::cerr << "Error: unable to redirect stdout" << std::endl;
	std::cerr << "System error " << strerror(errno) << std::endl;
	return -6;
      }
    }
    // std::cerr << "Done!" << std::endl;
    // std::cerr << "redirect stderr to " << stderr_ << "...";
    if (stderr_!="") {
      if ( freopen(stderr_.c_str(),"w",stderr) == NULL ) {
	std::cerr << "Error: unable to redirect stderr" << std::endl;
	std::cerr << "System error " << strerror(errno) << std::endl;
	return -7;
      }
    }
    // std::cerr << "Done!" << std::endl;

    // std::cerr << "running execv with arguments:" << std::endl;
    // std::cerr << argv[0] << " ";
    // for (unsigned int k=1; k<=args_.size(); k++)
    //   std::cerr << argv[k] << " ";
    // std::cerr << std::endl;

    // start user process
    if ( execv(argv[0],argv) == -1 ) {
      std::cerr << "Cannot start job, System error " << strerror(errno) << std::endl;
      std::cerr << argv[0] << " ";
      for (unsigned int k=1; k<=args_.size(); k++)
	std::cerr << argv[k] << " ";
      std::cerr << std::endl;
      return -8;
    }
    // execv doesn't return, so this point will be never reached
    exit(0);
    return 0;
  }
  ~BossExternalProcess() {}

};

#endif
