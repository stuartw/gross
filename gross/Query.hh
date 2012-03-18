#ifndef QUERY_H
#define QUERY_H
#include "Log.hh"

#include "Command.hh"
#include <vector>

class Query : public Command {
public:
  Query();
  ~Query();
  int execute();
  void printUsage() const;
private:
  void allTasks();
  vector<int> taskIds_;
};



#endif
