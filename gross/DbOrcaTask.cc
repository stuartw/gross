#include "DbOrcaTask.hh"

#include "TaskFactory.hh"
#include "Job.hh"
#include "CladLookup.hh"
#include "LocalDb.hh"


int DbOrcaTask::split() {
  if(!Id()) {
    cerr<<"DbOrcaTask::split() Error: Task has no ID"<<endl;
    return EXIT_FAILURE; //Task must be on database first
  }
  ostringstream os;
  os<< "TaskID="<<Id();
  vector<string> resDataSel, resJobId;
  if(LocalDb::instance()->tableRead("Analy_Job", "DataSelect", os.str(), resDataSel)) return EXIT_FAILURE;

  //Get from Database DataSelect and JobId for particular Task to create new job with
  for(vector<string>::const_iterator i = resDataSel.begin(); i!=resDataSel.end(); i++) {
    os.str("");
    os<< "TaskID="<<Id()<<"&&DataSelect="<<(*i);
    resJobId.clear();
    if(LocalDb::instance()->tableRead("Analy_Job", "JobID", os.str(), resJobId)) return EXIT_FAILURE;
    if(resJobId.size()>1) {
      cerr<<"DbOrcaTask::split() Error : found more than one job matching dataselection"<<endl;
      return EXIT_FAILURE;
    }	
    for(vector<string>::const_iterator j = resJobId.begin(); j!=resJobId.end(); j++) {
      if(Log::level()>0) cout <<"DbOrcaTask::split() Creating job with id " << (*j) << " for data selection (run) " << (*i)<<endl;
      Job* pJ = (TaskFactory::instance())->makeJob(atoi((*j).c_str()), atoi((*i).c_str()), this);
      if(!pJ) return EXIT_FAILURE; //check pointer exists
      if(!*pJ) return EXIT_FAILURE; //check object constructed ok
      jobs_.push_back(pJ);   //deleted in ~Task
    }
  }
  if(jobs_.empty()) {
    cerr<<"DbOrcaTask::split() Error: No jobs created for task "<< Id() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}