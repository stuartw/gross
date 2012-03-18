#ifndef BOSSIF_H
#define BOSSIF_H
#include "Log.hh"

#include<map>
class Task;
class Job;
#include "QInfoTask.hh"

/*!
  \brief BOSS interface class

  Interfaces to BOSS for all submission and monitoring commands that pass through BOSS. Using an interface to do this
  means that any changes to the BOSS interface are restricted to this class.

  

*/

class BossIf{
public:
  BossIf(const Task* pTask);
  int submitJob(const string aSched, const string aBossJobType, const Job* pJob) const; ///<To submit an individual job within a task
  int submitJobs(const string aSched, const string aBossJobType, int minJobId=0, int maxJobId=0);
  int submitTask(const string aSched, const string aBossJobType) const; ///<To submit all jobs within a task
  const string status(const Job* pJob); ///<Returns BOSS status letter (returns "U" for failure, "N" for not submitted)
  int bossId(const Job* pJob) const; ///<Returns 0 for error, BossId for success.
  const string schedId(const Job* pJob) const; ///<Returns "" for error, schedId for success.
  const string exitStatus(const Job* pJob) const; ///<Returns "" for error, job exit code for success.
  int killJob(const int jobId) const; ///<Returns 0 for error, 1 for success.
  int killTask() const; ///<Returns 0 for error, 1 for success.
  int killJobs(int minJobId, int maxJobId);
  int recoverJob(Job* job, const string journalDir);
private:
  /*!
    \brief General query method
    Note that this method is private as it should not be called within client code generally. 
    Friend permitted to allow for generic user query.
    Using this in general client code will break encapsulation of db schema within objects (ie each object
    responsible for its own schema in the db, in this case BossIf knows of the BOSS schema).
  */
  friend int QInfoTask::printBossDbField(const string) const;
  const string queryBossDb(const Job* pJob, const string tablename, 
			   const string field) const; ///<tablename must contain fKey (called ID for JOB table else JOBID) referring to bossId
  int saveId(const int aTaskId, const int aJobId, const int aBossId) const;
  const Task* pTask_;
  int minJob_;
  vector<Job*> jobs_;
  int maxJob_;
    
  //No implementation for
  BossIf(const BossIf&);
  BossIf& operator=(const BossIf&);
  typedef std::map<int, string> IntStringMap;
  IntStringMap jobStatusMap_;
};

#endif
