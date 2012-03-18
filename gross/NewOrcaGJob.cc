#include "NewOrcaGJob.hh"

#include "Task.hh"
#include "NewOrcaTask.hh"
#include "Wrapper.hh"
#include "JDL.hh"
#include "LocalDb.hh"
#include "FileSys.hh"
#include "File.hh"
#include "CladLookup.hh"
#include "TaskFactory.hh"

#include "PhysCat.hh"

NewOrcaGJob::NewOrcaGJob(const int anId, const int aDataSelect, Task* aTask) : 
  OrcaGJob(anId, aDataSelect, aTask), myNewOrcaTask_(0) {
  unInit_=true;
  
  //Need to downcast here to access full interface of NewOrcaTask.
  //Could consider having NewOrcaTask in ctor rather than Task, but then
  //would need to do messier things with upcasting to get base classes constructed.
  myNewOrcaTask_= dynamic_cast<NewOrcaTask*> (aTask);
  if(!myNewOrcaTask_) {
    cerr<<"NewOrcaGJob() Error: Task not defined correctly\n";
    return;
  }
  
  //The order matters here (eg you need to create the wrapper before you can set Job exec name, etc)
  if(setUniqSuffix()) return;
  if(setLocalInFiles()) return;
  if(setInGUIDs()) return;
  if(setStdOutFile()) return;
  if(setStdErrFile()) return;
  if(setOutSandboxFiles()) return;
  if(setOutGUIDs()) return;

  unInit_=false;
}

int NewOrcaGJob::save() {
  if(OrcaGJob::save()) return EXIT_FAILURE;
  
  //XMLFrag
  if(!setXMLFrag()) return EXIT_FAILURE;
  
  //Need escaped string of the file contents  
  string convS = LocalDb::instance()->escapeString((setXMLFrag())->contents());
  if(convS.empty()) return EXIT_FAILURE;
  
  ostringstream mySel;
  mySel << "TaskID="<<task_->Id() <<"&&JobID="<<Id();

  if(LocalDb::instance()->tableUpdate("Analy_Job", "XMLFrag", convS, mySel.str())) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
int NewOrcaGJob::makeSubFiles() {
  if(createOrcaFiles()) return EXIT_FAILURE;
  if(createWrapper()) return EXIT_FAILURE;
  if(createJDL()) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}


int NewOrcaGJob::createOrcaFiles() {
  //Orca XML File
  if(!setXMLFrag()) {
    cerr<<"NewOrcaGJob::createOrcaFiles Error - unable to create XML Fragment for this run"<<endl;
    return EXIT_FAILURE;
  }
  
  //XMLFile is a localInFile
  vLocalInFiles_.insert(xMLFrag());
  
  return EXIT_SUCCESS;
}
File* NewOrcaGJob::setXMLFrag() {
  if(pXMLFragFile_) return pXMLFragFile_;
  if(!myNewOrcaTask_->physCat()) {
    cerr<<"NewOrcaGJob::setXMLFrag Error! Task does not have PhysCat set!"<<endl;
    return 0;
  }
  string xMLFragName = FileSys::oDir() + string("/") + string("XMLFrag.xml")+ uniqSuffix();
  if(Log::level()>2) cout <<"NewOrcaGJob::setXMLFrag() Creating XMLFrag with name " << xMLFragName<<endl;
  ostringstream myQuery;
  myQuery<<"runid="<<dataSelect_;
  pXMLFragFile_ = (myNewOrcaTask_->physCat())->xMLFrag(myQuery.str(), xMLFragName);
  if(!pXMLFragFile_) return 0;

  //Store file as a local wrap file for the job
  vLocalWrapFiles_.insert(pXMLFragFile_);
  return pXMLFragFile_;
}

int NewOrcaGJob::createWrapper(){ //Always created "on the fly" (i.e. not persistent)
  string wrapFileName=string("orca.wrapper.sh")+ uniqSuffix();
  if(!wrapper_) wrapper_ = (TaskFactory::instance())->makeWrapper(this, (task_->userSpec()), wrapFileName);

  if(wrapper_->save(FileSys::oDir())) return EXIT_FAILURE;
  //Note that need LocalWrapFiles as well as LocalInFiles because wrap files are created
  //on the fly. So will be created again for DbJob. So can't be treated like LocalInFiles which
  //are saved to Db and re-read from Db by DbJob else would keep adding more and more entries for wrap files.
  for(set<File*>::const_iterator i=(wrapper_->files()).begin(); i!=(wrapper_->files()).end(); i++)
    vLocalWrapFiles_.insert(*i);
  
  setExecutable(); //Can only be done here, once wrapper has been created.

  return EXIT_SUCCESS;
}
int NewOrcaGJob::createJDL(){ //Always created "on the fly" (i.e. not persistent)
  string JDLFileName=string("analy.jdl") + uniqSuffix();
  if(!jdl_) jdl_ = (TaskFactory::instance())->makeJDL(this, (task_->userSpec()), JDLFileName);
  if(jdl_->save(FileSys::oDir())) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
int NewOrcaGJob::setUniqSuffix() {
  const string sS =(task_->userSpec())->read("OutFileSuffix"); 
  if(sS.empty()) {
    cerr<<"NewOrcaGJob::setUniqSuffix() Error: OutFileSuffix not defined in JDL"<<endl;
    return EXIT_FAILURE;
  }
  ostringstream os;
  os<< "." << sS << "_J" << Id();
  uniqSuffix_ = os.str();
  if(Log::level()>2) cout <<"NewOrcaGJob::setUniqSuffix() Setting uniqSuffix to " << uniqSuffix_<<endl;
  return EXIT_SUCCESS;
}
int NewOrcaGJob::setStdOutFile() {
  ostringstream os;
  const string sS = ((task_->userSpec())->read("StdOutput")); 
  if(sS.empty())
    os << "stdOut";
  os<< sS <<uniqSuffix();
  stdOutFile_ = os.str();
  if(Log::level()>2) cout <<"NewOrcaGJob::setStdOutFile() set to "<<stdOutFile_<<endl;
  return EXIT_SUCCESS;
}
int NewOrcaGJob::setStdErrFile() {
  ostringstream os;
  const string sS = ((task_->userSpec())->read("StdError")); 
  if(sS.empty())
    os<<"stdErr";
  os << sS << uniqSuffix();
  stdErrFile_ = os.str();
  if(Log::level()>2) cout <<"NewOrcaGJob::setStdErrFile() set to "<<stdErrFile_<<endl;
  return EXIT_SUCCESS;
}
int NewOrcaGJob::setExecutable() {
  executable_ = "Undefined";
  if(wrapper_)
    executable_ = wrapper_->execNameFullHandle();
  return EXIT_SUCCESS;
}
int NewOrcaGJob::setLocalInFiles(){
  //Set Local Input files
  vector<string> vS;
  (task_->userSpec())->read("InputSandbox", vS); //No need to check if defined or not - optional parameter
  for(vector<string>::const_iterator it = vS.begin(); it!=vS.end(); it++) {
    if(Log::level()>2) cout << "NewOrcaGJob::setLocalInFiles() reading input file name:" << (*it) << endl;
    vLocalInFiles_.insert(new File(*it));
  }    
  //don't forget user executable is local infile too!
  const string sF = (task_->userSpec())->read("Executable");  
  if(sF.empty()) {
    cerr <<"NewOrcaGJob::setLocalInFiles() Error Executable name not defined in JDL"<<endl;
    return EXIT_FAILURE;
  }
  vLocalInFiles_.insert(new File(sF));

  return EXIT_SUCCESS;
}
int NewOrcaGJob::setOutSandboxFiles(){
  //Set Output Sandbox Files
  //Output log files
  if(Log::level()>2) cout << "NewOrcaGJob::setOutSandboxFiles() adding "<<stdOutFile()<<endl;
  vOutSandboxFiles_.insert(stdOutFile());  
  if(Log::level()>2) cout << "NewOrcaGJob::setOutSandboxFiles() adding "<<stdErrFile()<<endl;
  vOutSandboxFiles_.insert(stdErrFile());

  //Output sandbox files
  vector<string> sVS;
  (task_->userSpec())->read("OutputSandbox", sVS); //Optional parameter - no need to ensure it's been defined
  
  for(vector<string>::const_iterator it = sVS.begin(); it!=sVS.end(); it++) {
    if(Log::level()>2) cout << "NewOrcaGJob::setOutSandboxFiles() reading output file name:" << (*it) << endl;
    ostringstream newfilename;
    newfilename << (*it) << uniqSuffix();
    if(Log::level()>2) cout << "NewOrcaGJob::setOutSandboxFiles() setting output GUID to:" << newfilename.str() << endl;
    vOutSandboxFiles_.insert(newfilename.str());
  } 
  return EXIT_SUCCESS;
}
int NewOrcaGJob::setOutGUIDs(){
  //Set Output File GUIDs
  //..nothing as yet. Only OutSandboxFiles catered for.
  // Space here for where user *knows* name of output files to be registered remotely (ie not part of output sandbox)
  return EXIT_SUCCESS;
}
int NewOrcaGJob::setInGUIDs(){
  //Set Input Data Files from Phys Cat
  ostringstream myQuery;
  myQuery<<"runid="<<dataSelect_;
  if(!myNewOrcaTask_->physCat()) {
    cerr<<"NewOrcaGJob::setInGUIDs Error! Task does not have PhysCat set!"<<endl;
    return EXIT_FAILURE;
  }
  const vector<string> myFiles = (myNewOrcaTask_->physCat())->listLFNs(myQuery.str());
  for(vector<string>::const_iterator i = myFiles.begin(); i!= myFiles.end() ; i++){
    if(Log::level()>2) cout<<"NewOrcaGJob::setInGUIDs() setting inGUID to " << (*i)<<endl;
    vInGUIDs_.insert(*i);
  }

  //Set Input Meta Data File from Phys Cat
  string query("DataType='ZippedMETA'");
  const vector<string> tmp = (myNewOrcaTask_->physCat())->listLFNs(query);
  if(tmp.size()!=1) {
    cerr << "NewOrcaGJob::setInGUIDs Error: Meta Data File not found in catalog!"<<tmp.size() << endl;
    return EXIT_FAILURE;
  }
  for(vector<string>::const_iterator it = tmp.begin(); it!=tmp.end(); ++it) 
    metaFile_=(*it);

  return EXIT_SUCCESS;
}

