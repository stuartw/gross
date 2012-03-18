#include "OrcaLocWrapper.hh"

#include "CladLookup.hh"
#include "OrcaLocJob.hh"
#include "File.hh"
#include "ContPrint.hh"

OrcaLocWrapper::OrcaLocWrapper(const OrcaLocJob* myJob, CladLookup* myUserSpec, const string myExecName, const string myType) 
  : Wrapper(myExecName, myType), job_(myJob), userSpec_(myUserSpec) {};

int OrcaLocWrapper::steer(){
  steer_="";
  ostringstream osbuff;

  if(!job_->uniqSuffix().empty()) osbuff << "Suffix " << job_->uniqSuffix() <<endl;

  if(job_->xMLFrag()) osbuff << "XMLFragFile " << (job_->xMLFrag())->fullHandle() <<endl;

  if(!job_->outDir().empty()) osbuff << "OutDir " << job_->outDir() <<endl;


  //UserExecutable (ensure file exists)
  const string sF = userSpec_->read("Executable");
  if(sF.empty()) {
    cerr<<"Wrapper::steer() Error Executable not defined in JDL"<<endl;
    return EXIT_FAILURE;
  }
  const File fF(sF);    
  if(!fF.exists()) {
    cerr<<"Wrapper::steer() Error User Exec file does not exist "<< fF.fullHandle()<<endl;
    return EXIT_FAILURE;
  }
  osbuff << "userExec "<<fF.fullHandle()<<endl;

  //Owner and Dataset
  const string sOwner = userSpec_->read("Owner");
  const string sDataset = userSpec_->read("Dataset");
  if(sOwner.empty()||sDataset.empty()) {
    cerr <<"Wrapper::steer() Error Owner/Dataset not defined in jdl!" <<endl;
    return EXIT_FAILURE;
  }  
  osbuff << "Owner "<<sOwner<<endl;
  osbuff << "Dataset "<<sDataset<<endl;  
  
  //Data Selection
  vector<int> runs=job_->dataSelect();
  for (vector<int>::const_iterator i=runs.begin(); i!= runs.end(); i++)
    osbuff << "Run " << (*i) <<endl;
  
  //outFiles - add suffix to user given names in wrapper
  set<string> sVS;
  sVS=job_->outFiles();
  for (set<string>::const_iterator i=sVS.begin(); i!= sVS.end(); i++)
    osbuff<<"OutFile " << (*i) <<endl;
    
  //localInFiles
  for (set<File*>::const_iterator i=(job_->localInFiles()).begin(); i!= (job_->localInFiles()).end(); i++)
    osbuff<<"localFile " << (*i)->fullHandle() <<endl;
    
  //inFiles
  for (set<string>::const_iterator i=(job_->inFiles()).begin(); i!= (job_->inFiles()).end(); i++)
    osbuff<<"InFile " << (*i) <<endl;

  //Dar Setup file
  const string sD = userSpec_->read("DarSetupFile");
  if (sD.empty()) {
    cerr<<"Wrapper::steer() Error DarSetupFile not defined in JDL"<<endl;
    return EXIT_FAILURE;
  }
  osbuff << "DarSetupFile " << sD << endl;

  
  steer_=osbuff.str();
  return EXIT_SUCCESS;
}
