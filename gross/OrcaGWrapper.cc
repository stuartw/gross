#include "OrcaGWrapper.hh"

#include "CladLookup.hh"
#include "OrcaGJob.hh"
#include "File.hh"

OrcaGWrapper::OrcaGWrapper(const OrcaGJob* myJob, CladLookup* myUserSpec, const string myExecName, const string myType) 
  : Wrapper(myExecName, myType), job_(myJob), userSpec_(myUserSpec) {};

int OrcaGWrapper::steer(){
  steer_="";
  ostringstream osbuff;

  const string sS = userSpec_->read("ORCAVersion");
  if(sS.empty()) {
    cerr<<"Wrapper::steer() Error ORCAVersion not defined in JDL"<<endl;
    return EXIT_FAILURE;
  }
  osbuff << "orcaVers "<< sS <<endl;

  if(!job_->uniqSuffix().empty()) osbuff << "Suffix " << job_->uniqSuffix() <<endl;

  if(job_->xMLFrag()) osbuff << "XMLFragFile " << (job_->xMLFrag())->name() <<endl;

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
  osbuff << "userExec "<<fF.name()<<endl;

  //Owner and Dataset
  const string sOwner = userSpec_->read("Owner");
  const string sDataset = userSpec_->read("Dataset");
  if(sOwner.empty()||sDataset.empty()) {
    cerr <<"Wrapper::steer() Error Owner/Dataset not defined in jdl!" <<endl;
    return EXIT_FAILURE;
  }  
  osbuff << "Owner "<<sOwner<<endl;
  osbuff << "Dataset "<<sOwner<<endl;  

  //MetaData File
  const string meta_= job_->metaFile();
  if(meta_.empty()) {
    cerr<<"Wrapper::steer() Error Meta File not defined!"<<endl;
    return EXIT_FAILURE;
  }
  osbuff << "MetaDataFile " << meta_ <<endl;
  
  //outSandboxFileNames - note that need to add suffix to whatever filename user lists in UserSpec
  vector<string> sVS;
  if(userSpec_->read("OutputSandbox", sVS)) {
    cerr<<"Wrapper::steer() Error OutputSandbox not defined in JDL"<<endl;
    return EXIT_FAILURE;
  }    

  for (vector<string>::const_iterator i=sVS.begin(); i!= sVS.end(); i++)
    osbuff<<"OutputSandboxFile " << (*i) <<endl;
    
  //localInFiles
  for (set<File*>::const_iterator i=(job_->localInFiles()).begin(); i!= (job_->localInFiles()).end(); i++)
    osbuff<<"localInFile " << (*i)->name() <<endl;
    
  //inGUIDs
  for (set<string>::const_iterator i=(job_->inGUIDs()).begin(); i!= (job_->inGUIDs()).end(); i++)
    osbuff<<"RemoteDataInputFile " << (*i) <<endl;
  
  steer_=osbuff.str();
  return EXIT_SUCCESS;
}