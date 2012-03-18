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
#include "ContPrint.hh"
#include "pstream.h"
#include "StringSpecial.hh"
#include "PhysCat.hh"


using namespace stringSpecial;


NewOrcaGJob::NewOrcaGJob(const int anId, const vector<int> aDataSelect, Task* aTask) : 
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
  if(setOrcaVers()) return;
  if(setExecutable()) return;
  if(setUniqSuffix()) return;
  if(setSites()) return;
  if(setLocalInFiles()) return;
  if(setStdOutFile()) return;
  if(setStdErrFile()) return;
  if(setOutSandboxFiles()) return;
  if(setOutGUIDs()) return;

  unInit_=false;
}

int NewOrcaGJob::save() {
  if(OrcaGJob::save()) return EXIT_FAILURE;
  
  //tarFile
  if(setOrcaVers()) return EXIT_FAILURE;
  if(setExecutable()) return EXIT_FAILURE;
  if(!setTarFile()) return EXIT_FAILURE;
 

  return EXIT_SUCCESS;
}
int NewOrcaGJob::makeSubFiles() {
  cout << "Creating files for job id " << Id() << endl;
  if(createOrcaFiles()) return EXIT_FAILURE;
  if(setSites()) return EXIT_FAILURE;
  if(createWrapper()) return EXIT_FAILURE;
  if(createJDL()) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
int NewOrcaGJob::delSubFiles() {
  if (wrapper_) {
    for(set<File*>::const_iterator i=(wrapper_->files()).begin(); i!=(wrapper_->files()).end(); i++)
      (*i)->remove();
  }
  if (pTarFile_) pTarFile_->remove();
  if (jdl_) {
    File file(jdl_->fullHandle());
    if (file.exists()) file.remove();
  }
  return EXIT_SUCCESS;
}
int NewOrcaGJob::createOrcaFiles() {
   
  if(setOrcaVers()) {
    cerr << "NewOrcaGJob::setOrcaVers Error - unable to determine required ORCA version" << endl;
    return EXIT_FAILURE;
  }
  
  if(!setTarFile()) {
    cerr << "NewOrcaGJob::setTarFile Error - unable to create tar of executable and libraries" <<endl;
    return EXIT_FAILURE;
  }
  vLocalWrapFiles_.insert(pTarFile_);
	  
  return EXIT_SUCCESS;
}
File* NewOrcaGJob::setTarFile() {
  if(pTarFile_) return pTarFile_;
  ostringstream tarName;
  tarName << FileSys::oDir() << "/" << executable_ << "_" << orcaVers_  << "_" << getpid() << string(".tar.gz");  
  if(Log::level()>2) cout << "NewOrcaGJob::SetTarFile() Creating Tar file " << tarName.str() << endl;
  pTarFile_ = new File(tarName.str());
  if (pTarFile_->exists()) return pTarFile_;
  
  string ArchCommand = "scram arch";
  redi::ipstream psub3(ArchCommand);
  string output; 
  string ARCH;
  while(std::getline(psub3,output)) {
    if(stringSearch(output,"error")) {
      cerr << "NewOrcaGJob::SetTarFile Error unable to determine SCRAM arch " << output << endl;
      return 0;
    }
    ARCH = output;
    if(Log::level()>3) cout << "NewOrcaGJob::setTarFile() Found SCRAM arch " << ARCH<<endl;
  }
  psub3.clear();
  output.clear();
  string LocalRT = getenv("LOCALRT");
  if (ARCH==""||LocalRT=="") return 0;
  string exeName = LocalRT + string("/bin/") + ARCH + string("/") + executable_;
  //check exe exits - exit if not found 
  File exe(exeName);
  if (!(exe.exists())) {
    cerr << "NewOrcaGJob::SetTarFile() Error unable to locate exe " << exe.fullHandle() <<endl;
    return 0;
  }
  
  string libCommand = string("ldd ") + exeName;
  string cdCommand1 = string("cd ") + LocalRT + string(";");
  string working = FileSys::workingDir();
  string cdCommand2 = string("cd ") + working + string(";");
  redi::ipstream psub1(libCommand, redi::pstreambuf::pstdout|redi::pstreambuf::pstderr);
  string libs;
  
  if(psub1.eof()) {
    cerr << "NewOrcaGJob::SetTarFile Error unable to locate executable: " << exeName << endl;
    return 0;
  }
  while(std::getline(psub1, output)) {
      if(stringSearch(output,"not found")) {
      cerr << "NewOrcaGJob::SetTarFile Error unable to locate all libraries: " << output << endl;
      return 0;
    }
    if(stringSearch(output,LocalRT)) { 
      string sTemp=output.substr(1,output.find("=>")-2);
      if(Log::level()>2) cout << "NewOrcaGJob::setTarFile() Found user library " << sTemp <<endl;
      libs = libs+string(" ") + string("lib/") + ARCH + string ("/") + sTemp;
    }
  }
  psub1.close();
  output.clear();
  string tarCommand = cdCommand1 +string("tar zcf ") + tarFile()->fullHandle() + libs + string(" bin/") + ARCH + string("/") + executable_ + string("; ") + cdCommand2;
  redi::ipstream psub2(tarCommand, redi::pstreambuf::pstdout|redi::pstreambuf::pstderr); 
  while(std::getline(psub2, output)) {
    if(stringSearch(output,"error")) {
      cerr<<"NewOrcaGJob::setTarFile An error has occurred in the command "<<tarCommand <<" " << output<<endl;
      return 0;
    }
  }
  psub2.close();
  //store as local wrap file for job

  return pTarFile_;
}
int NewOrcaGJob::setOrcaVers() {
  if (orcaVers_!="") return EXIT_SUCCESS;
  char* pc=getenv("LOCALRT");
  if (!pc) {
    cerr << "NewOrcaGJob::setOrcaVers Error! Unable to find scram path" << endl;
    cerr << "please run \"eval `scram runtime <-sh|-csh>`\"" << endl;
    return EXIT_FAILURE;
  }
  string LocalRT(pc);
  int pos = LocalRT.rfind("/");
  if (pos==string::npos) {
    cerr << "Error: unable to determine SCRAM area" << endl;	 
    return EXIT_FAILURE; 
  } else if (pos==LocalRT.size()-1) {
    int end=pos-1;
    pos=LocalRT.rfind("/", pos-1);
    orcaVers_=LocalRT.substr(pos+1,end-pos);
  } else { 
    orcaVers_=LocalRT.substr(pos+1, LocalRT.size()-pos);
  }
  if (Log::level()>2) cout << "NewOrcaGJob::setOrcaVers found ORCA version: " << orcaVers_<<endl;
  return EXIT_SUCCESS;
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
  string JDLFileName=string("orca.jdl") + uniqSuffix();
  if(!jdl_) jdl_ = (TaskFactory::instance())->makeJDL(this, (task_->userSpec()), JDLFileName);
  if(jdl_->save(FileSys::oDir())) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
int NewOrcaGJob::setUniqSuffix() {
  const string sS =(task_->userSpec())->read("OutFileSuffix"); 
  if(sS.empty()) {
    //allow empty suffix
    //cerr<<"NewOrcaGJob::setUniqSuffix() Error: OutFileSuffix not defined in JDL"<<endl;
    //return EXIT_FAILURE;
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
  if (executable_!="") return EXIT_SUCCESS;
  executable_ = "Undefined";
  executable_ = task_->userSpec()->read("Executable");
  if (executable_=="Undefined") return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
int NewOrcaGJob::setLocalInFiles(){
  //Set Local Input files
  vector<string> vS;
  //Need an orcarc definned
  if (task_->userSpec()->read("Orcarc", vS) || vS.size()!=1 ) {
    cerr << "NewOrcaGJob::setLocalInFiles() Error unable to determine orcarc from jdl" <<endl;
    return EXIT_FAILURE;
  }
  vLocalInFiles_.insert(new File(vS[0]));
  vS.clear(); 
  (task_->userSpec())->read("InputSandbox", vS); //No need to check if defined or not - optional parameter
  for(vector<string>::const_iterator it = vS.begin(); it!=vS.end(); it++) {
    if(Log::level()>2) cout << "NewOrcaGJob::setLocalInFiles() reading input file name:" << (*it) << endl;
    vLocalInFiles_.insert(new File(*it));
  }    
  
  for(set<File*>::const_iterator it=vLocalInFiles_.begin(); it!=vLocalInFiles_.end(); it++) {
    if ( !(*it)->exists() ) {
      cerr << "NewOrcaGJob::setLocalInFiles() Error unable to find file: " << (*it)->fullHandle() << endl;
      return EXIT_FAILURE;
    }
  }
  
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
    if((*it).empty()) continue;
    if(Log::level()>2) cout << "NewOrcaGJob::setOutSandboxFiles() reading output file name:" << (*it) << endl;
    ostringstream newfilename;
    newfilename << (*it) << uniqSuffix();
    if(Log::level()>2) cout << "NewOrcaGJob::setOutSandboxFiles() setting output name to:" << newfilename.str() << endl;
    vOutSandboxFiles_.insert(newfilename.str());
  } 
  return EXIT_SUCCESS;
}
int NewOrcaGJob::setOutGUIDs(){
  vector<string> vS;
  (task_->userSpec())->read("OutputFileLFN", vS); //No need to check if defined or not - optional parameter
  for(vector<string>::const_iterator it = vS.begin(); it!=vS.end(); it++) {
    if(Log::level()>2) cout << "NewOrcaGJob::setOutGUIDs() reading output file name:" << (*it) << endl;
    vOutGUIDs_.insert(*it);
}
      
  return EXIT_SUCCESS;
}
/*int NewOrcaGJob::setInGUIDs(){
  //Set Input Data Files from Phys Cat
  ostringstream myQuery;
  myQuery <<"runid=";
  for (int i=0;i<dataSelect().size();i++) {
    if (i<(dataSelect().size()-1)) myQuery<<dataSelect()[i]<<" OR runid=";
    else myQuery<<dataSelect_[i];
  } 
  if(!myNewOrcaTask_->physCat()) {
    cerr<<"NewOrcaGJob::setInGUIDs Error! Task does not have PhysCat set!"<<endl;
    return EXIT_FAILURE;
  }
  const vector<string> myFiles = (myNewOrcaTask_->physCat())->listLFNs(myQuery.str());
  for(vector<string>::const_iterator i = myFiles.begin(); i!= myFiles.end() ; i++){
    if(Log::level()>2) cout<<"NewOrcaGJob::setInGUIDs() setting inGUID to " << (*i)<<endl;
    vInGUIDs_.insert(*i);
  }
  return EXIT_SUCCESS;
}*/
int NewOrcaGJob::setSites() {
  if(Log::level()>2) cout << "NewOrcaGJob::setSites() finding PubDBs with this dataset" << endl;
  const NewOrcaTask* mytask_ = dynamic_cast<const NewOrcaTask*> (task());
  vSites = mytask_->physCat()->getSites4MyRuns(dataSelect());
  if (vSites.empty()) {
    cerr << "NewOrcaGJob::setSites() Error No PubDb's found with this dataset" << endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
