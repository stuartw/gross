#ifndef DELETE_H
#define DELETE_H
#include "Log.hh"

#include "Command.hh"

class Delete : public Command {
public:
  Delete();
  ~Delete();
  int execute();
  void printUsage() const;
};



#endif
