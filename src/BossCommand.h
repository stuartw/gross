// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossCommand.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_COMMAND_H
#define BOSS_COMMAND_H

#include <map>
#include <string>

class BossCommand {
protected:
  typedef std::map< std::string,std::string,std::less<std::string> > Options;
  typedef Options::const_iterator Options_const_iterator;
  typedef Options::iterator Options_iterator;
  Options opt_;
public:
  BossCommand();
  virtual ~BossCommand();
  int acceptOptions(int,char**);
  virtual int execute() =0;
  virtual void printUsage() const =0;
};

#endif
