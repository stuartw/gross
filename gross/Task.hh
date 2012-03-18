#ifndef TASK_H
#define TASK_H
#include "Log.hh"

class Job;
class CladLookup;

/*!
  \brief pABC that represents an analysis task 
  
  A user submits to GROSS a single, unique analysis task.
  This task will consist of a certain set of user requirements which are defined in a CladLookup object. 
  Task's responsibility is to carry out job splitting - creating and initialising the resultant 
  Job objects, creating any necessary local files ready for submission and saving itself (and its
  daughter Job objects).
  
  See UML diagram for inheritance structure class diagram. This pABC determines interface for a generic
  task. The next level down in the inheritance structure represents whether the object is to be created
  from a new user specification or from the database. The important difference between these two sub-classes
  is that job splitting for a new task will (most likely) involve communicating with a data catalogue of some
  sort, whereas for a Task already made persistent on a database, this (potentially time consuming and time
  dependent) activity will have already been completed. Further sub-classing below this is done for the 
  actual concrete task implementation specific for a particular job type.

  Note on design: as you go down the inheritance tree, sub-classes obtain additional attributes and 
  behaviour. These are defined at the first level in which they are appropriate. So a PhysCat object
  is appropriate only for any new Orca type of task (NewOrca) and will only be defined and initialised 
  within the NewOrca sub-class - furthermore, the getter for this attribute will only be defined for this
  sub-class. This extends the generic Task interface defined here.
*/

class Task {
public:
  Task(); ///< Base class ctor just initialises own variables to sensible values.
  virtual ~Task() = 0; ///< pABC

  /*!
    \brief Check initialisation of object has been completed successfully.
    
    Client classes must check that ctor succeeded using this method. This is required
    as the derived class ctors may well fail - eg they might contact remote DBs. 
    For this to work, derived class ctors must set unInit_ to false after their ctor
    has successfully initialised the derived object. Note that derived classes of derived classes 
    must set the flag to false themselves at the beginning of their ctor and then to true once their ctor
    has successfully completed.
    Exceptions could also be used here, but this is easiest for the moment.
  */
  bool operator!() {if(unInit_) cerr <<"Task::operator!() Error: task initialisation failure!\n"; return unInit_;}

  virtual int split() = 0; ///< %Job splitting method

  /*!
    \brief Creates submission files for jobs

    This method will create all submission files that are required for each daughter job
    and (if appropriate) for the Task itself.
    This will include, for example, the Wrapper script steering file and a JDL file
    to drive the BOSS submission process.
    
    \retval 0 if save failed
    \retval >0 Id of Task on database (Note Id_ is set to this once Task is saved)
    
  */
  virtual int makeSubFiles() = 0;


  /*!
    \brief Saves Task and its Job objects to the database
    
    For consistency, both Task and its daughter jobs are saved at the same time with this method.
    Thus jobs must be created first otherwise the save will return an error.
    
    \retval 0 if save failed
    \retval >0 Id of Task on database (Note Id_ is set to this once Task is saved)

   */
  virtual int save() = 0;


  const int Id() const {return Id_;} ///<Getter

  CladLookup* userSpec() const {return userSpec_;} ///<Getter


  const vector<Job*>* jobs() const {return &jobs_;} ///<Getter 

  /*!
    \brief Getter for single Job from Task
    
    Returns pointer to a single job, given a particular job Id.
    
    \return pointer to Job object returned normally or null pointer is returned in case of error
    
    \warning no error message printed should null pointer be returned - client must test for empty pointer.
  */
  const Job* job(int anId);

protected:
  /*!
    \brief The daughter Job objects of this Task    
    
    \todo Shouldn't have a data member accessible like this. But what to do with vectors? 
  */
  vector<Job*> jobs_;

  void userSpec(CladLookup* myUserSpec) {userSpec_=myUserSpec;} ///<Setter

  void Id(int myId) {Id_=myId;} ///<Setter

  void userClad(string myUserClad) {userClad_=myUserClad;} ///<Setter

  const string userClad() const {return userClad_;} ///<Setter

  /*!
    \brief Save jobs method

    Implementation in Base class rather than derived classes, as will be same for most derived classes.
  */
  virtual int saveJobs()=0;

  bool unInit_; ///<For use with operator!()

private:
  CladLookup* userSpec_;
  int Id_;
  string userClad_;

  Task(const Task&); ///< No implementation
  Task& operator=(const Task&); ///< No implementation
};

#endif
