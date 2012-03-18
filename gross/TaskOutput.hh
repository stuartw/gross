#ifndef TASKOUTPUT_H
#define TASKOUTPUT_H

#include "Log.hh"
class Task;
class Job;

class TaskOutput {
public:
  TaskOutput();
  int init(int taskId, int minJobId=0, int maxJobId=0);
  /*! \brief Prints local directories of Sboxs for all status finished jobs. If jobs not cleared, will retrieve from Grid.*/
  int printSboxDirs(string oDir="./") const;
  /*! \brief Returns local dir for Sbox of job. Job must have status finished. If job not cleared, will retrieve from Grid*/
  string getSbox(Job* pJob, string oDir="./") const;
private:
  Task* task_;
  int minJob_;
  int maxJob_;
  vector<Job*> allJobs_;
  vector<Job*> finJobs_;

  string getDbSbox(Job* pJob) const; //Read from Db
  string getGridSbox(Job* pJob, string oDir="./") const; //Retrieve from Grid
  int saveSboxDir(Job* pJob, string sboxDir) const; //Save to Db
  
  //No implementation
  TaskOutput(const TaskOutput&);
  TaskOutput operator=(const TaskOutput&);
};

#endif
