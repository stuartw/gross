#ifndef DELETE_H
#define DELETE_H
#include "Log.hh"

#include "Command.hh"

/*!
  \brief Delete Command
  
  Note that the code within this command violates the database encapsulation architecture 
  (namely that all database access for an object is carried out by the object itself). This 
  is for the good reason that delete may well want to delete tasks/job data on the database that
  can no longer be used to initialise task/job objects. This might be the case, for example, if
  the data has become corrupted or if the data was written by an older version of GROSS. To avoid
  this, the delete simply deletes everything with the TaskID (or JobID) in all tables listed.

*/


class Delete : public Command {
public:
  Delete();
  ~Delete();
  int execute();
  void printUsage() const;
};



#endif
