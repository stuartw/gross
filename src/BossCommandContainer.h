// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossCommandContainer.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_COMMAND_CONTAINER_H
#define BOSS_COMMAND_CONTAINER_H

#include <string>
#include <map>

class BossCommand;

class BossCommandContainer {
private:
  typedef std::map< std::string,BossCommand*,std::less<std::string> > Commands;
  typedef Commands::const_iterator Commands_const_iterator;
  typedef Commands::iterator Commands_iterator;
  Commands cmd_;
public:
  BossCommandContainer();
  ~BossCommandContainer();
  BossCommand* command(const std::string) const ;
  void printUsage() const; 
};

#endif
