#ifndef QINFOTASK_H
#define QINFOTASK_H

#include "Log.hh"
class Task;
class Job;

class QInfoTask {
public:
  QInfoTask();
  ~QInfoTask();
  int init(int taskId, int minJobId=0, int maxJobId=0);
  int printTask() const;
  int printJobs() const;
  int printDataQuery() const;
  int printStatus() const;
  int printSumStatus() const;
  int printBossDbField(const string field) const;
  void header(bool setting) {header_= setting;};
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
