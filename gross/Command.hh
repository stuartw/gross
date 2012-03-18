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
  /*!
    \brief gets the factory type from database
    
    Note that this breaks the encapsulation of db schema - should have all info for objects
    read from/stored on database by objects themselves. In this case however, the object 
    must be created using this information.
  */
  virtual string getFacType(int aTaskId) const;
};

#endif
