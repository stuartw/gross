#ifndef OUTPUT_H
#define OUTPUT_H
#include "Log.hh"

#include "Command.hh"

class Output : public Command {
public:
  Output();
  ~Output();
  int execute();
  void printUsage() const;
private:
  bool cont;
  bool init;
};



#endif
