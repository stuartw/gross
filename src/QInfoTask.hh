#ifndef QINFOTASK_H
#define QINFOTASK_H

#include "Log.hh"
class Task;
class Job;

/*!
  \brief Query information about a saved task and its jobs

  This helper class is useful because a user may only want to query a specific subset of 
  a task's jobs. Thus by initialising appropriately, it contains only the required jobs to
  be queried.

  The operations themselves print out static (ie database) and dynamic (ie dynamic BOSS query) based information.

  All formatting of print output is encapsulated here.

  Note that this is a simple but ugly (inflexible, un-OO, etc) way to provide the monitoring, but at least the 
  monitoring part is there. The main point is the information is there on the database. How a user chooses 
  to access it is up to him/her.
*/

class QInfoTask {
public:
  QInfoTask();
  ~QInfoTask();
  int init(int taskId, int minJobId=0, int maxJobId=0);
  int printTask() const; ///<Print all static task information
  int printJobs() const; ///<Print static job information
  int printDataQuery() const; ///<Print out data query for task and each individual jobs
  int printStatus() const; ///<Print status
  int printSumStatus() const; ///<Print summary of status of all jobs within task
  int printBossDbField(const string field) const; ///<Print specific user defined value from within database
  void header(bool setting) {header_= setting;}; ///<Turn header on/off. Used when printing multiple task/job information as a table.
private:
  Task* task_;
  int minJob_;
  int maxJob_;
  vector<Job*> jobs_;
  bool header_;

  //No implementation
  QInfoTask(const QInfoTask&);
  QInfoTask operator=(const QInfoTask&);
};
#endif
