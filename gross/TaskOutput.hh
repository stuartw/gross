#ifndef TASKOUTPUT_H
#define TASKOUTPUT_H

#include "Log.hh"
class Task;
class Job;

/*!
  \brief Class that gathers output from the job

  Class will no doubt contain more options in the future, but currently able just to retrieve the output sandbox
  from the Grid to a particular local directory, or if already retrieved it prints out the location of the 
  output files (or at least where GROSS put them when it first retrieved them).

  This is one operation that breaks the general architecture model of only talking to the Grid through BOSS. However as 
  BOSS provides no functionality to retrieve output, we have to cut across it and do it ourselves. So it's a bit messy
  but the only way to do it. Also, the LCG libraries are so complicated (a huge number to link), 
  that it is easier to simply call command line LCG operations and parse the output. Very fragile regarding future changes
  to the LCG CLI, but again pragmatism has dictated it.
  
*/

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

  string getDbSbox(Job* pJob) const; ///<Read from Db
  string getGridSbox(Job* pJob, string oDir="./") const; ///<Retrieve from Grid
  int saveSboxDir(Job* pJob, string sboxDir) const; ///<Save to Db
  
  //No implementation
  TaskOutput(const TaskOutput&);
  TaskOutput operator=(const TaskOutput&);
};

#endif
