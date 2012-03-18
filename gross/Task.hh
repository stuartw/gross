#ifndef TASK_H
#define TASK_H
#include "Log.hh"

class Job;
class CladLookup;
class PhysCat;
class File;

/*!
  \brief pABC that represents an analysis Task 
  
  A user submits to GROSS a single, unique analysis task.
  This task will consist of a certain set of user requirements which are defined in a CladLookup object 
  and a set of job objects. The Task class's responsibility is to create and initialise the Job objects.
  In addition, it must be able to save itself (and delete) to a database.
  
  Task is currently sub-classed into DbTask and NewTask families. 
  It is expected that in the future further sub-classing, down the inheritance tree from these two sub-classes,
  will be implemented to define different methods of job splitting and job initialisation.

  \todo instead of having aType (Job type) passed around, have this as a Task parameter (or Job parameter on creation of Job).
  It's a pain to have to pass it round as an argument all the time - it's just another parameter really.
  \todo Get rid of queryJobs() and replace with a new Query object which will handle all query functionality.

*/
class Task {
public:
  Task(); ///< Ctor sets default parameters.
  virtual ~Task() = 0; ///< Virtual dtor as pABC
  /*! 
    \brief Initialisation method
    
    Initialisation of Task separated from ctor, to enable safe error handling (particularly
    as initialisation involves reading from a file).
    
    Initialisation of Task will cover creation of CladLookup object and setting of some data members.

    One of the following parameters or the other is required for initialisation (never both).
    Note that cannot overload this with two functions each with single type argument, 
    as derived classes override it.

    \param userCladFile is a pointer to the user specification ClassAd file
    \param anId is the Task Id.

    \todo see if you can set up defaults for the arguments to init() to make this overloaded call less messy.
  */
  virtual int init(const File* userCladFile, const int& anId) = 0;
  /*!
    \brief Job splitting method

    \param aType is the Job type to be created
  */
  virtual int prepareJobs(const string& aType) = 0; ///< Job splitting method
  /*!
    \brief Saves Task and its Job objects to the database
    
    For consistency, both Task and Jobs are saved at the same time with this method. 
    Thus jobs must be created first otherwise the save will return an error.
    
    \retval 0 if save failed
    \retval >0 Id of Task on database (Note Id_ is set to this once Task is saved)

    \todo for neatness and consistency, have a saveTask protected member function (like saveJobs).
    This would decouple the two in a nice way in case of future changes (eg overriding of either function).
   */
  virtual int save() = 0;

  /*!
    \brief Should only be called for DbTask class 
    
    Provides minimal task and job initialisation, sufficient for a query to be carried out (no files are created)
  */
  virtual int queryPrepareJobs(); //Should only be called for DbTask class

  /*!
    \brief Getter for Task Id
  */
  const int Id() const {return Id_;};
  /*!
    \brief Getter for Task's CladLookup
  */
  CladLookup* userSpec() const {return userSpec_;};
  /*!
    \brief Getter for PhysCat 
  */
  PhysCat* physCat() const {return physCat_;};
  /*! 
    \brief Getter for vector of Task's Job objects
  */
  const vector<Job*>* jobs() const {return &jobs_;};
  /*!
    \brief Getter for single Job from Task
    
    Returns pointer to a single job, given a particular job Id.
    
    \return pointer to Job object returned normally or null pointer is returned in case of error
    
    \warning no error message printed should null pointer be returned - client must test for empty pointer.
  */
  const Job* job(int anId); //Single job in task with Id given by client //Test for empty pointer returned in case of error (with no error msg)

protected:
  /*!
    \brief The daughter Job objects of this Task    
    
    \todo Shouldn't have a data member accessible like this. But what to do with vectors? 
  */
  vector<Job*> jobs_;

  /*!
    \brief Setter for CladLookup object
    
    Use this setter rather than allowing inheritance of base class data members
   */
  void userSpec(CladLookup* myUserSpec) {userSpec_=myUserSpec;};
  /*!
    \brief Setter for Task Id
    
    Use this setter rather than allowing inheritance of base class data members
   */
  void Id(int myId) {Id_=myId;};
  /*!
    \brief Setter for PhysCat
  */
  void physCat(PhysCat* myPhysCat) {physCat_=myPhysCat;};
  /*!
    \brief Setter for User Specification Clad
  */
  void userClad(string myUserClad) {userClad_=myUserClad;};
  /*!
    \brief Getter for User Specification Clad
  */
  const string userClad() const {return userClad_;};

  /*! 
    \brief Creates all submission files
  */
  virtual int createSubFiles(const string& aType);
  /*!
    \brief  Saves Job objects
  */
  virtual int saveJobs() = 0;
private:
  CladLookup* userSpec_;
  int Id_;
  PhysCat* physCat_;
  string userClad_;

  Task(const Task&); ///< No implementation
  Task& operator=(const Task&); ///< No implementation
};

#endif
