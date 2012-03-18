#ifndef TASKFACTORY_H
#define TASKFACTORY_H

#include "Log.hh"

class Task;
class Job;
class File;
class JDL;
class Wrapper;
class CladLookup;

/*!
  \brief (Singleton) Abstract factory to create family of classes appropriate for job type

  This abstract factory is implemented as a singleton. Client classes can simply call the 
  instance() method in order to instantiate the appropriate concrete factory which contains
  the correct implementation of the factory methods (eg makeTask, makeJob, etc). The type
  of concrete factory to be created is set with the facType() method which must be called 
  first. 

  A specific concrete factory will create a certain combination of Task, Job, Wrapper and 
  JDL. Constructors for these classes are only called from this factory and nowhere else in the
  programme code. It is up to the concrete factory coder to 
  ensure that the mix of classes makes sense together! The name of the concrete factory should be 
  identical to the name of the wrapper script 
  that is registered on the database - this is to simplify the user's choices (though, inevitably
  this also reduces the flexibility of the programme). 

  Note that this is not quite a singleton class in that a single instance does not necessarily
  last throughout program runtime - it can be deleted and re-instantiated with a different 
  concrete factory type, as is required if (for example) several different types of task
  need to be created. Note also that the factory is not responsible for deleting the objects that
  it has created - this is done by the client classes (%Task in this case).

  Design note: one of the problems with using the factory method to produce derived classes 
  (eg NewOrcaGJob) is that the factory methods themselves - the interface - refer to base
  class objects for generality, whereas the extended interface of a created derived class might be needed by
  client classes (this is true for NewOrcaGJob, for example, when used with the OrcaGJDL class). 
  To get around this, appropriate casts need to be used - it is preferable to put these centrally in 
  the concrete factory for safety.
  
  See UML diagram for class hierarchy details.

*/
  

class TaskFactory {
public:
  
  virtual ~TaskFactory()=0; ///<pABC 
  static TaskFactory* instance(); ///<Return concrete factory instance
  static void del(); ///<Delete concrete factory instance. Note that only removes instance, not objects created by instance.

  static void facType(string myFacType); ///<Setter for Factory Type   
  static string facType() {return facType_;} ///<Getter for Factory Type

  virtual Task* makeTask(const File* userCladFile) const=0; ///<Factory method
  virtual Task* makeTask(int anId) const=0; ///<Factory method
  //virtual Job* makeJob(const int anId, const int aDataSelect, Task* aTask) const=0; ///<Factory method
  virtual Job* makeJob(const int anId, const vector<int> aDataSelect, Task* aTask) const=0; ///<Factory method
  virtual JDL* makeJDL(const Job* aJob, CladLookup* aUserSpec, const string aFileName) const=0; ///<Factory method
  virtual Wrapper* makeWrapper(const Job* aJob, CladLookup* aUserSpec, const string aFileName) const=0; ///<Factory method
protected:
  TaskFactory() {}; ///<Singleton
private:
  static string facType_; ///<Factory type indicator
  static TaskFactory* instance_; ///<Concrete factory instance
};


#endif
