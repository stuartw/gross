#ifndef COMMAND_CONTAINER_H
#define COMMAND_CONTAINER_H
#include "Log.hh"

#include <map>

class Command;

class CommandContainer {
private:
  typedef std::map< string,Command*,std::less<string> > Commands;
  typedef Commands::const_iterator Commands_const_iterator;
  typedef Commands::iterator Commands_iterator;
  Commands cmd_;
public:
  CommandContainer();
  ~CommandContainer();
  Command* command(const string) const ;
  void printUsage() const; 
};

#endif
