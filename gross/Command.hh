#ifndef COMMAND_H
#define COMMAND_H
#include "Log.hh"

#include <map>
class DbSpec;

class Command {
protected:
  typedef std::map< string,string,std::less<string> > Options;
  typedef Options::const_iterator Options_const_iterator;
  typedef Options::iterator Options_iterator;
  Options opt_;
  DbSpec* dbSpec_;
public:
  Command();
  virtual ~Command();
  int acceptOptions(int,char**);
  virtual int execute() =0;
  virtual void printUsage() const =0;
  virtual int initDb();
};

#endif
