#include "Wrapper.hh"

#include "CladLookup.hh"
#include "Job.hh"
#include "LocalDb.hh"
#include "File.hh"


Wrapper::Wrapper(const Job* myJob, CladLookup* myUserSpec, const string myExecName, const string myType) 
  : job_(myJob), userSpec_(myUserSpec), execName_(myExecName), execNameFullHandle_(myExecName), type_(myType){
}
Wrapper::~Wrapper() {
  filesCreated_.~vector<File*>();
}
const int Wrapper::save(string myDir) {
  filesCreated_.clear();

  //Save Script file
  if(Log::level()>0) cout << "Wrapper::save() Saving script file " << execName_ <<endl;
  execNameFullHandle_=myDir+"/"+execName_;
  File* myScriptFile = new File(execNameFullHandle_); //deleted in ~Wrapper
  if(this->script()) return EXIT_FAILURE;
  if(myScriptFile->save(script_)) return EXIT_FAILURE;
  filesCreated_.push_back(myScriptFile);
  
  //Make script executable
  if(myScriptFile->makeExec()) return EXIT_FAILURE;

  //Save Steer file
  string steerFileName=execName_ + ".steer";
  if(Log::level()>0) cout << "Wrapper::save() Saving steer file " << steerFileName <<endl;
  File* mySteerFile = new File((myDir+"/"+steerFileName)); //deleted in ~Wrapper
  if(this->steer()) return EXIT_FAILURE;
  if(mySteerFile->save(steer_)) return EXIT_FAILURE;
  filesCreated_.push_back(mySteerFile);

  return EXIT_SUCCESS;
}
int Wrapper::steer(){
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
  for (vector<File*>::const_iterator i=(job_->localInFiles()).begin(); i!= (job_->localInFiles()).end(); i++)
    osbuff<<"localInFile " << (*i)->name() <<endl;
    
  //inGUIDs
  for (vector<string>::const_iterator i=(job_->inGUIDs()).begin(); i!= (job_->inGUIDs()).end(); i++)
    osbuff<<"RemoteDataInputFile " << (*i) <<endl;
  
  steer_=osbuff.str();
  return EXIT_SUCCESS;
}
int Wrapper::script() {
  script_="";
  ostringstream os;
  os<<"name="<<"'"<<type_<<"'";
  vector<string> myResults;
  if(LocalDb::instance()->tableRead("Analy_Wrapper", "script", os.str(), myResults)) return EXIT_FAILURE;
  if(myResults.size()!=1) {
    cerr<<"Wrapper::script() Error retrieving from db wrapper script named "<<type_<<endl;
    return EXIT_FAILURE;
  }
  script_=myResults[0];
  return EXIT_SUCCESS;
}
