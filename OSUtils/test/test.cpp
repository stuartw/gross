// /////////////////////////////////////////////////////////////////////
// Program: test
// Version: 1.0
// File:    test.cpp
// Authors: Claudio Grandi (INFN BO)
// Date:    15/03/2004
// Note:
// /////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>

//#include "pstream.h"
#include<sstream>

#include "OperatingSystem.h"
#include "ExternalProcess.h"
#include "ExampleProcess.h"

int main(int argc, char** argv) {

  // fork an external process
//   std::string exec = "/bin/cat";
//   std::string stdin = "test.in";
//   std::string stdout = "test.out"; 
//   std::string stderr = ""; // if empty it is not redefined
//   std::string args = "-E"; // add $ at end of line (just to check args) 
  std::string exec = "/bin/ls";
  std::string stdin = "";
  std::string stdout = "test.out"; 
  std::string stderr = "test.err";
  std::string args = "pippo";
  OSUtils::Process* comm = new OSUtils::ExternalProcess(exec,stdin,stdout,stderr,args);

  // this process just sleeps a given amount of seconds
  OSUtils::Process* example = new OSUtils::ExampleProcess(5);

  // this process just sleeps a given amount of seconds
  OSUtils::Process* example2 = new OSUtils::ExampleProcess(100);
  example2->setName("Very Long wait process");

  // start the processes
  OSUtils::forkProcess(comm);
  OSUtils::forkProcess(example);
  OSUtils::forkProcess(example2);

  // this wait is blocking
  OSUtils::waitProcess(comm);
  std::cout << comm->name() << " exited with code " 
	    << comm->retCode() << std::endl;

  // this check allows to build user-defined logic
  while(OSUtils::checkProcess(example)=="running") {
    std::cout << example->name() << " still running..." << std::endl;
    OSUtils::sleep(1);
  }
  std::cout << example->name() << " exited with code " 
	    << example->retCode() << std::endl;

  // this wait is non-blocking. Process killed if it takes more than maxWait
  int maxWait = 10;
  OSUtils::waitProcessMaxTime(example2,maxWait);
  std::cout << example2->name() << " exited with code " 
	    << example2->retCode() << std::endl;

  // another wait may be added to OperatingSystem that returns immediately
  // cleanup
  delete comm;
  delete example;
  delete example2;

  // use  ipstream
  std::string command = "ls -l";
  //  std::stringstream comm_out;
  //  OSUtils::getCommandOutput(command,comm_out);
  //  redi::ipstream comm_out(command.c_str());
  OSUtils::CommandStream comm_out(command.c_str());
  std::string buffer;
  while (comm_out) {
    getline(comm_out,buffer,'\n'); // '\n' is optional as 3rd argument
    std::cout << "processing line: " << buffer << std::endl;
  }

  return 0;
}
