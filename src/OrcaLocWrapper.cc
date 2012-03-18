#include "OrcaLocWrapper.hh"

#include "CladLookup.hh"
#include "OrcaLocJob.hh"
#include "File.hh"
#include "ContPrint.hh"
#include "Site.hh"

OrcaLocWrapper::OrcaLocWrapper(const OrcaLocJob* myJob, CladLookup* myUserSpec, const string myExecName, const string myType) 
  : Wrapper(myExecName, myType), job_(myJob), userSpec_(myUserSpec) {};

int OrcaLocWrapper::steer(){
  steer_="";
  ostringstream osbuff;

  const string orca=job_->orcaVers();
  if (orca=="") {
    return EXIT_FAILURE;
  }
  osbuff << "orcaVers "<< orca <<endl;
      
  
  if(!job_->uniqSuffix().empty()) osbuff << "Suffix " << job_->uniqSuffix() <<endl;

  /*if(job_->outDir().empty()) {
    cerr << "OrcaLocWrapper Error - No output directory(OutDir) specified in jdl" <<endl;
    return EXIT_FAILURE;
  }  
  osbuff << "OutDir " << job_->outDir() <<endl;
  */
  //UserExecutable (ensure file exists)
  const string sF = job_->executable();
  if(sF.empty()) {
    cerr<<"Wrapper::steer() Error Executable not defined in JDL"<<endl;
    return EXIT_FAILURE;
  }
  osbuff << "userExec "<<sF<<endl;

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
  File file(sOrcarc);
  if (!file.exists()) {
    cerr << "Wrapper::steer() Error orcarc not found " << sOrcarc << endl;
    return EXIT_FAILURE;
  }
  osbuff << "Orcarc " << file.name()<<endl;
      
  
  //scram
  const string sScram = userSpec_->read("Scram");
  if(sScram.empty()) {
    cerr << "Wrapper::steer() Error scram location(Scram) not defined in jdl" << endl;
    return EXIT_FAILURE;
  }
  osbuff << "Scram " << sScram <<endl; 
    
  //Data Selection
  vector<int> runs=job_->dataSelect();
  for (vector<int>::const_iterator i=runs.begin(); i!= runs.end(); i++)
    osbuff << "Run " << (*i) <<endl;
  
  //outFiles - add suffix to user given names in wrapper
  vector<string> sVS;
  //sVS=job_->outFiles();
  userSpec_->read("OutputSandbox", sVS);
  for (vector<string>::const_iterator i=sVS.begin(); i!= sVS.end(); i++)
    osbuff<<"OutFile " << (*i) <<endl;
  
    
  //localInFiles
  for (set<File*>::const_iterator i=(job_->localInFiles()).begin(); i!= (job_->localInFiles()).end(); i++)
    osbuff<<"localFile " << (*i)->name() <<endl;
    
  //wrapFiles
  for (set<File*>::const_iterator i=(job_->localWrapFiles()).begin(); i!= (job_->localWrapFiles()).end(); i++)
    osbuff<<"localFile " << (*i)->name() <<endl;
  

  //inFiles
  //for (set<string>::const_iterator i=(job_->inFiles()).begin(); i!= (job_->inFiles()).end(); i++)
  //  osbuff<<"InFile " << (*i) <<endl;

  //Dar Setup file
  /*const string sD = userSpec_->read("DarSetupFile");
  if (sD.empty()) {
    cerr<<"Wrapper::steer() Error DarSetupFile not defined in JDL"<<endl;
    return EXIT_FAILURE;
  }
  osbuff << "DarSetupFile " << sD << endl;
  */
  
  //PubDB POOL catalogue contact strings
  //still needed to get POOL cat for this local site
  //even if not submitted via grid
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
