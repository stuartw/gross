// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossLabellerProcess.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_LABELLER_PROCESS_H
#define BOSS_LABELLER_PROCESS_H

#include <string>
#include <fstream>

#include "BossProcess.h"
#include "BossOperatingSystem.h"
#include "BossUpdateElement.h"

class BossLabellerProcess : public BossProcess {
private:
  int id_;
  std::string type_;
  std::string inpipe_;
  std::string outfile_;
public:
  BossLabellerProcess(int id, std::string type, std::string in, std::string out) : 
    BossProcess(), id_(id), type_(type), inpipe_(in), outfile_(out) {}

  int start() {
    BossOperatingSystem* sys=BossOperatingSystem::instance();
    // Check input pipe
    if ( ! sys->fileExist(inpipe_) ) {
      std::cerr << "BossLabeller: Input pipe " << inpipe_ 
	   << " does not exist. Abort." << std::endl;
      return -1;
    }
    // std::ifstream inp(inpipe_.c_str());
    // The new fstream assumes buffered I/O (~ 8K: too big!)
    // Try a work around:
    std::ifstream inp;
    char buf;
    inp.rdbuf()->pubsetbuf(&buf,1); // 1 char buffer
    inp.open(inpipe_.c_str());
    /*
    the following requires #include <ext/stdio_filebuf.h>:
    __gnu_cxx::stdio_filebuf<char> b(std::fopen(inpipe_.c_str(),"r"),std::ios_base::in,1);
    std::istream inp(&b);
    */
    //
    if ( !inp ) {
      std::cerr << "BossLabeller: Unable to open input pipe " << inpipe_ 
	   << " Abort." << std::endl;
      return -3;
    }
    // Check output file
    if ( ! sys->fileExist(outfile_) ) {
      std::cerr << "BossLabeller: output journal file " << outfile_ 
	   << " does not exist. Abort." << std::endl;
      return -2;
    }
#ifdef LOGL2
    std::cout << "Executing: labeller process " << std::endl;
    std::cout << "     PID : " << sys->getPid() << std::endl;
    std::cout << "      In : " << inpipe_ << std::endl;
    std::cout << "      Out: " << outfile_ << std::endl;
#endif
    while (!inp.eof()) {
      std::string buffer;
      char ch = inp.peek();
      while ( ch == '#' || ch == '\n' ) { // skip comment and empty lines
	getline(inp,buffer);
	ch = inp.peek();
      }
      if ( ch == EOF ) break; // exit if EOF
      getline(inp,buffer,'=');
      sys->trim(buffer);
      std::string ident = buffer;
      if ( buffer == "" ) continue;
      getline(inp,buffer,'\n');
      sys->trim(buffer);
      std::string value = buffer;
      if ( buffer == "" ) continue;
      BossUpdateElement ue(id_,type_,ident,value);
      sys->append(outfile_,ue.str());
      // DEBUG
      // std::cerr << "BossLabellerProcess writing " << ue << std::endl;
      // END DEBUG
    }

    return 0;
  }
  ~BossLabellerProcess() {}


};

#endif
