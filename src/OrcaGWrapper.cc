#include "OrcaGWrapper.hh"

#include "CladLookup.hh"
#include "OrcaGJob.hh"
#include "File.hh"
#include "ContPrint.hh"
#include "Site.hh"

OrcaGWrapper::OrcaGWrapper(const OrcaGJob* myJob, CladLookup* myUserSpec, const string myExecName, const string myType) 
  : Wrapper(myExecName, myType), job_(myJob), userSpec_(myUserSpec) {};

int OrcaGWrapper::steer(){
  steer_="";
  ostringstream osbuff;

  const string orca=job_->orcaVers();
  //const string orca="ORCA_8_7_1";
  if (orca=="") {
    return EXIT_FAILURE;
  }
  
  osbuff << "orcaVers "<< orca <<endl;

  if(!job_->uniqSuffix().empty()) osbuff << "Suffix " << job_->uniqSuffix() <<endl;

  //SCRAM build executable
  /* Will implement later
  string sSCRAM = userSpec_->read("SCRAMBuildEXE");
  if (sSCRAM.empty()) sSCRAM="false";
  osbuff << "SCRAMBuildEXE " << sSCRAM <<endl;
        
  //remotly located executable
  string sREMOTE = userSpec_->read("RemoteEXE");
  if (sREMOTE.empty()) sREMOTE="false";
  osbuff << "RemoteEXE " << sREMOTE <<endl;
  */

  //UserExecutable (ensure file exists)
  const string sEXE = job_->executable();
  if(sEXE.empty()) {
    cerr<<"Wrapper::steer() Error Executable not defined in JDL"<<endl;
    return EXIT_FAILURE;
  }
  osbuff << "userExec "<< sEXE <<endl;
  
  //executable arguments
  const string sArg = userSpec_->read("Arguments");
  if (!sArg.empty()) osbuff << "Arguments " << sArg << endl;
  
  //Owner and Dataset
  const string sOwner = userSpec_->read("Owner");
  const string sDataset = userSpec_->read("Dataset");
  if(sOwner.empty()||sDataset.empty()) {
    cerr <<"Wrapper::steer() Error Owner/Dataset not defined in jdl!" <<endl;
    return EXIT_FAILURE;
  }  
  osbuff << "Owner "<<sOwner<<endl;
  osbuff << "Dataset "<<sDataset<<endl;  
 
  //orcarc
  const string sOrcarc = userSpec_->read("Orcarc");
  if (sOrcarc.empty()) {
    cerr << "Wrapper::steer() Error orcarc not defined in jdl" <<endl;
    return EXIT_SUCCESS;
  }
  File file(sOrcarc);
  if (!file.exists()) { 
    cerr << "Wrapper::steer() Error orcarc not found " << sOrcarc << endl;
    return EXIT_FAILURE;
  } 
  osbuff << "Orcarc " << file.name()<<endl;
  
  
  //Data Selection
  vector<int> runs=job_->dataSelect();
  for (vector<int>::const_iterator i=runs.begin(); i!= runs.end(); i++)
    osbuff << "Run " << (*i) <<endl;
  
  //outSandboxFileNames - note that need to add suffix to whatever filename user lists in UserSpec
  vector<string> sVS;
  if(userSpec_->read("OutputSandbox", sVS)) {
    //allow no output files as stdout and err automatically added
    //cerr<<"Wrapper::steer() Error OutputSandbox not defined in JDL"<<endl;
    //return EXIT_FAILURE;
  }    

  for (vector<string>::const_iterator i=sVS.begin(); i!= sVS.end(); i++)
    osbuff<<"OutputSandboxFile " << (*i) <<endl;
    
  //localInFiles
  for (set<File*>::const_iterator i=(job_->localInFiles()).begin(); i!= (job_->localInFiles()).end(); i++) {
    osbuff<<"localInFile " << (*i)->name()  <<endl;
  }

  //wrapFiles - needed to access tar file on worker node
  for (set<File*>::const_iterator i=(job_->localWrapFiles()).begin(); i!= (job_->localWrapFiles()).end(); i++) {
    osbuff<<"localInFile " << (*i)->name()  <<endl;
  }

  
  //inGUIDs
  //for (set<string>::const_iterator i=(job_->inGUIDs()).begin(); i!= (job_->inGUIDs()).end(); i++)
  //  osbuff<<"RemoteDataInputFile " << (*i) <<endl;
  
  //outGUIDs
  for (set<string>::const_iterator i=(job_->outGUIDs()).begin(); i!= (job_->outGUIDs()).end(); i++)
    osbuff<<"RemoteDataOutputFile " << (*i) <<endl;

  //PubDB POOL catalogue contact strings
  vector<Site*> sites = job_->sites();
  for (vector<Site*>::const_iterator i=sites.begin(); i!=sites.end(); i++) {
    vector<string> CEs = (*i)->getCE();
    vector<string> Cat = (*i)->getContactString();
    osbuff << endl << CEs[0]; 
    for (vector<string>::const_iterator i=Cat.begin(); i!=Cat.end(); i++) {
      osbuff << " " << *i;
    }
  }

  steer_=osbuff.str();
  return EXIT_SUCCESS;
}
