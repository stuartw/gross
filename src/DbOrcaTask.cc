#include "DbOrcaTask.hh"

#include "TaskFactory.hh"
#include "Job.hh"
#include "CladLookup.hh"
#include "LocalDb.hh"
#include "PhysCat.hh"

int DbOrcaTask::split() {
  if(!Id()) {
    cerr<<"DbOrcaTask::split() Error: Task has no ID"<<endl;
    return EXIT_FAILURE; //Task must be on database first
  }

  //get owner and dataset
  const string sOwner = userSpec()->read("Owner");
  const string sDataset = userSpec()->read("Dataset");
  const string sDataChain = userSpec()->read("DataChain"); //may be empty
  if(sOwner.empty()||sDataset.empty()) {
    cerr <<"DbOrcaTask::split() Error: Dataset Owner and/or Dataset name not defined in JDL in DB(Owner=?/Dataset=?)"<<endl;
    return EXIT_FAILURE;
  }

  //Initialise catalogue - this will create temporary local cat
  physCat(new PhysCat(sOwner, sDataset, sDataChain));  //deleted in ~DbOrcaTask  
  ostringstream os;
  os<< "TaskID="<<Id();
  vector<string> resDataSel, resJobId;
  if(LocalDb::instance()->tableRead("Analy_Job", "DataSelect", os.str(), resDataSel)) return EXIT_FAILURE;
    
  //Get from Database DataSelect and JobId for particular Task to create new job with
  for(vector<string>::const_iterator i = resDataSel.begin(); i!=resDataSel.end(); i++) {
    os.str("");
    os<< "TaskID="<<Id()<<"&&DataSelect="<<"\""<<(*i)<<"\"";
    resJobId.clear();
    if(LocalDb::instance()->tableRead("Analy_Job", "JobID", os.str(), resJobId)) return EXIT_FAILURE;
    if(resJobId.size()>1) {
      cerr<<"DbOrcaTask::split() Error : found more than one job matching dataselection"<<endl;
      return EXIT_FAILURE;
    }
    
    //parse DataSelection and load individual runs
    for(vector<string>::const_iterator j = resJobId.begin(); j!=resJobId.end(); j++) {
      vector<int> runs;
      for (int x=0, y=(*i).size(); x < (*i).size();) {
        if ((y = (*i).find(",", x)) != string::npos) {
	} else {
	x = (*i).size();
	}
	int diff = y - x;
	runs.push_back(atoi( (*i).substr(x, diff).c_str() ));
	x = y + 1;
      } 
	    
      if(Log::level()>0) cout <<"DbOrcaTask::split() Creating job with id " << (*j) << " for data selection (runs) " << (*i)<<endl;
      Job* pJ = (TaskFactory::instance())->makeJob(atoi((*j).c_str()), runs, this);
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
