#include "NewJob.hh"

#include <BossCommandInterpreter.h>
#include "Task.hh"
#include "Wrapper.hh"
#include "JDL.hh"
#include "LocalDb.hh"
#include "FileSys.hh"
#include "File.hh"
#include "CladLookup.hh"

#include "PhysCat.hh"

NewJob::~NewJob() {
  if(pXMLFragFile_) delete pXMLFragFile_;
}

//First explicitly inherit from Job base class the following
int NewJob::save() {
  if(Job::save()) return EXIT_FAILURE; //Must be called first.
  if(this->saveOrca()) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
int NewJob::clean() {return Job::clean();};

//Member functions
NewJob::NewJob(const int anId, const int aDataSelect, Task* aTask) : Job(anId, aDataSelect, aTask), pXMLFragFile_(0) {};

int NewJob::init(){
  //The order matters here (eg you need to create the wrapper before you can set Job exec name, etc)
  if(this->setUniqSuffix()) return EXIT_FAILURE;
  if(this->setLocalInFiles()) return EXIT_FAILURE;
  if(this->setInGUIDs()) return EXIT_FAILURE;
  if(this->setStdOutFile()) return EXIT_FAILURE;
  if(this->setStdErrFile()) return EXIT_FAILURE;
  if(this->setOutSandboxFiles()) return EXIT_FAILURE;
  if(this->setOutGUIDs()) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

int NewJob::createSubFiles(string myType) {
  if(this->createOrcaFiles()) return EXIT_FAILURE;
  if(this->createWrapper(myType)) return EXIT_FAILURE;
  if(this->createJDL()) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
int NewJob::createOrcaFiles() {
  //Orca XML File
  if(!this->setXMLFrag()) {
    cerr<<"NewJob::createOrcaFiles Error - unable to create XML Fragment for this run"<<endl;
    return EXIT_FAILURE;
  }

  //XMLFile is a localInFile
  vLocalInFiles_.push_back(xMLFrag());

  return EXIT_SUCCESS;
}
File* NewJob::setXMLFrag() {
  if(pXMLFragFile_) return pXMLFragFile_;
  if(!task_->physCat()) {
    cerr<<"NewJob::setXMLFrag Error! Task does not have PhysCat set!"<<endl;
    return 0;
  }
  string xMLFragName = FileSys::oDir() + string("/") + string("XMLFrag.xml")+ this->uniqSuffix();
  if(Log::level()>2) cout <<"NewJob::setXMLFrag() Creating XMLFrag with name " << xMLFragName<<endl;
  ostringstream myQuery;
  myQuery<<"runid="<<dataSelect_;
  pXMLFragFile_ = (task_->physCat())->xMLFrag(myQuery.str(), xMLFragName);
  if(!pXMLFragFile_) return 0;

  //Store file as a local wrap file for the job
  vLocalWrapFiles_.push_back(pXMLFragFile_);
  return pXMLFragFile_;
}
int NewJob::saveOrca() {
  //Separate off the different saves coming from different 
  //specialisations of the Job class. Rather than having one global save
  //for whole Job class. In future sub-classes will almost certainly have different data
  //to save.

  //Note Job::save() Should be called first as it has safety functions
  //within it to ensure unique ID, etc within Job table.

  //Job Table
  if(!this->setXMLFrag()) return EXIT_FAILURE;

  //Need escaped string of the file contents  
  string convS = LocalDb::instance()->escapeString((this->setXMLFrag())->contents());
  if(convS.empty()) return EXIT_FAILURE;

  ostringstream select;
  select<<"TaskID="<<this->task_->Id()<<"&&JobID="<<this->Id();

  if(LocalDb::instance()->tableUpdate("Analy_Job", "XMLFrag", convS, select.str())) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

int NewJob::createWrapper(string myType){ //Always created "on the fly" (i.e. not persistent)
  string wrapFileName=string("orca.wrapper.sh")+ this->uniqSuffix();
  if(!wrapper_) wrapper_ = new Wrapper(this, (task_->userSpec()), wrapFileName, myType);
  if(wrapper_->save(FileSys::oDir())) return EXIT_FAILURE;
  //Note that need LocalWrapFiles as well as LocalInFiles because wrap files are created
  //on the fly. So will be created again for DbJob. So can't be treated like LocalInFiles which
  //are saved to Db and re-read from Db by DbJob else would keep adding more and more entries for wrap files.
  for(vector<File*>::const_iterator i=(wrapper_->files()).begin(); i!=(wrapper_->files()).end(); i++)
    vLocalWrapFiles_.push_back(*i);
  
  setExecutable(); //Can only be done here, once wrapper has been created.

  return EXIT_SUCCESS;
}
int NewJob::createJDL(){ //Always created "on the fly" (i.e. not persistent)
  string JDLFileName=string("analy.jdl") + this->uniqSuffix();
  if(!jdl_) jdl_ = new JDL(this, (task_->userSpec()), JDLFileName);
  if(jdl_->save(FileSys::oDir())) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
int NewJob::setUniqSuffix() {
  const string sS =(task_->userSpec())->read("OutFileSuffix"); 
  if(sS.empty()) {
    cerr<<"NewJob::setUniqSuffix() Error: OutFileSuffix not defined in JDL"<<endl;
    return EXIT_FAILURE;
  }
  ostringstream os;
  os<< "." << sS << "_J" << this->Id();
  uniqSuffix_ = os.str();
  if(Log::level()>2) cout <<"NewJob::setUniqSuffix() Setting uniqSuffix to " << uniqSuffix_<<endl;
  return EXIT_SUCCESS;
}
int NewJob::setStdOutFile() {
  ostringstream os;
  const string sS = ((task_->userSpec())->read("StdOutput")); 
  if(sS.empty())
    os << "stdOut";
  os<< sS <<this->uniqSuffix();
  stdOutFile_ = os.str();
  if(Log::level()>2) cout <<"NewJob::setStdOutFile() set to "<<stdOutFile_<<endl;
  return EXIT_SUCCESS;
}
int NewJob::setStdErrFile() {
  ostringstream os;
  const string sS = ((task_->userSpec())->read("StdError")); 
  if(sS.empty())
    os<<"stdErr";
  os << sS << this->uniqSuffix();
  stdErrFile_ = os.str();
  if(Log::level()>2) cout <<"NewJob::setStdErrFile() set to "<<stdErrFile_<<endl;
  return EXIT_SUCCESS;
}
int NewJob::setExecutable() {
  executable_ = "Undefined";
  if(wrapper_)
    executable_ = wrapper_->execNameFullHandle();
  return EXIT_SUCCESS;
}
int NewJob::setLocalInFiles(){
  //Set Local Input files
  vector<string> vS;
  (task_->userSpec())->read("InputSandbox", vS); //No need to check if defined or not - optional parameter
  for(vector<string>::const_iterator it = vS.begin(); it!=vS.end(); it++) {
    if(Log::level()>2) cout << "NewJob::setLocalInFiles() reading input file name:" << (*it) << endl;
    vLocalInFiles_.push_back(new File(*it));
  }    
  //don't forget user executable is local infile too!
  const string sF = (task_->userSpec())->read("Executable");  
  if(sF.empty()) {
    cerr <<"NewJob::setLocalInFiles() Error Executable name not defined in JDL"<<endl;
    return EXIT_FAILURE;
  }
  vLocalInFiles_.push_back(new File(sF));

  return EXIT_SUCCESS;
}
int NewJob::setOutSandboxFiles(){
  //Set Output Sandbox Files
  //Output log files
  if(Log::level()>2) cout << "NewJob::setOutSandboxFiles() adding "<<this->stdOutFile()<<endl;
  vOutSandboxFiles_.push_back(stdOutFile());  
  if(Log::level()>2) cout << "NewJob::setOutSandboxFiles() adding "<<this->stdErrFile()<<endl;
  vOutSandboxFiles_.push_back(stdErrFile());

  //Output sandbox files
  vector<string> sVS;
  (task_->userSpec())->read("OutputSandbox", sVS); //Optional parameter - no need to ensure it's been defined
  
  for(vector<string>::const_iterator it = sVS.begin(); it!=sVS.end(); it++) {
    if(Log::level()>2) cout << "NewJob::setOutSandboxFiles() reading output file name:" << (*it) << endl;
    ostringstream newfilename;
    newfilename << (*it) << uniqSuffix();
    if(Log::level()>2) cout << "NewJob::setOutSandboxFiles() setting output GUID to:" << newfilename.str() << endl;
    vOutSandboxFiles_.push_back(newfilename.str());
  } 
  return EXIT_SUCCESS;
}
int NewJob::setOutGUIDs(){
  //Set Output File GUIDs
  //..nothing as yet. Only OutSandboxFiles catered for.
  // Space here for where user *knows* name of output files to be registered remotely (ie not part of output sandbox)
  return EXIT_SUCCESS;
}
int NewJob::setInGUIDs(){
  //Set Input Data Files from Phys Cat
  ostringstream myQuery;
  myQuery<<"runid="<<dataSelect_;
  if(!task_->physCat()) {
    cerr<<"NewJob::setInGUIDs Error! Task does not have PhysCat set!"<<endl;
    return EXIT_FAILURE;
  }
  const vector<string> myFiles = (task_->physCat())->listLFNs(myQuery.str());
  for(vector<string>::const_iterator i = myFiles.begin(); i!= myFiles.end() ; i++){
    if(Log::level()>2) cout<<"NewJob::setInGUIDs() setting inGUID to " << (*i)<<endl;
    vInGUIDs_.push_back(*i);
  }

  //Set Input Meta Data File from Phys Cat
  string query("DataType='ZippedMETA'");
  const vector<string> tmp = (task_->physCat())->listLFNs(query);
  if(tmp.size()!=1) {
    cerr << "NewJob::setInGUIDs Error: Meta Data File not found in catalog!"<<tmp.size() << endl;
    return EXIT_FAILURE;
  }
  for(vector<string>::const_iterator it = tmp.begin(); it!=tmp.end(); ++it) 
    metaFile_=(*it);

  return EXIT_SUCCESS;
}
