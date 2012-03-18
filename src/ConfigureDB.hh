#ifndef CONFIGUREDB_H
#define CONFIGUREDB_H
#include "Log.hh"

#include "Command.hh"

class ConfigureDB : public Command {
public:
  ConfigureDB();
  ~ConfigureDB();
  int execute();
  void printUsage() const;
};



#endif
