#include "NewOrcaLocJob.hh"

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

NewOrcaLocJob::NewOrcaLocJob(const int anId, const vector<int> aDataSelect, Task* aTask) : 
  OrcaLocJob(anId, aDataSelect, aTask), myNewOrcaTask_(0) {
  unInit_=true;
  
  //Need to downcast here to access full interface of NewOrcaTask.
  //Could consider having NewOrcaTask in ctor rather than Task, but then
  //would need to do messier things with upcasting to get base classes constructed.
  myNewOrcaTask_= dynamic_cast<NewOrcaTask*> (aTask);
  if(!myNewOrcaTask_) {
    cerr<<"NewOrcaLocJob() Error: Task not defined correctly\n";
    return;
  }
  
  //The order matters here (eg you need to create the wrapper before you can set Job exec name, etc)
  if(setUniqSuffix()) return;
  if(setLocalInFiles()) return;
  if(setInFiles()) return;
  if(setStdOutFile()) return;
  if(setStdErrFile()) return;
  if(setOutFiles()) return;
  if(setOutDir()) return; 
 
  unInit_=false;
}

int NewOrcaLocJob::save() {
  if(OrcaLocJob::save()) return EXIT_FAILURE;
  
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
int NewOrcaLocJob::makeSubFiles() {
  if(createOrcaFiles()) return EXIT_FAILURE;
  if(createWrapper()) return EXIT_FAILURE;
  if(createJDL()) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}


int NewOrcaLocJob::createOrcaFiles() {
  //Orca XML File
  if(!setXMLFrag()) {
    cerr<<"NewOrcaLocJob::createOrcaFiles Error - unable to create XML Fragment for this run"<<endl;
    return EXIT_FAILURE;
  }
  
  //XMLFile is a localInFile
  vLocalInFiles_.insert(xMLFrag());
  
  return EXIT_SUCCESS;
}
File* NewOrcaLocJob::setXMLFrag() {
  if(pXMLFragFile_) return pXMLFragFile_;
  if(!myNewOrcaTask_->physCat()) {
    cerr<<"NewOrcaLocJob::setXMLFrag Error! Task does not have PhysCat set!"<<endl;
    return 0;
  }
  string xMLFragName = FileSys::oDir() + string("/") + string("XMLFrag.xml")+ uniqSuffix();
  if(Log::level()>2) cout <<"NewOrcaLocJob::setXMLFrag() Creating XMLFrag with name " << xMLFragName<<endl;
  ostringstream myQuery;
  myQuery<<"runid=";
    for (int i=0;i<dataSelect_.size();i++) {
      if (i<(dataSelect_.size()-1)) myQuery<<dataSelect()[i]<<" OR runid=";	
      else myQuery<<dataSelect_[i];
    }
  myQuery<<" OR DataType='META' OR pfname='PersilCatalogInit'";
  pXMLFragFile_ = (myNewOrcaTask_->physCat())->xMLFrag(myQuery.str(), xMLFragName);
  if(!pXMLFragFile_) return 0;

  //Store file as a local wrap file for the job
  vLocalWrapFiles_.insert(pXMLFragFile_);
  return pXMLFragFile_;
}

int NewOrcaLocJob::createWrapper(){ //Always created "on the fly" (i.e. not persistent)
  string wrapFileName=string("wrapper.sh")+ uniqSuffix();
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
int NewOrcaLocJob::createJDL(){ //Always created "on the fly" (i.e. not persistent)
  string JDLFileName=string("submit.jdl") + uniqSuffix();
  if(!jdl_) jdl_ = (TaskFactory::instance())->makeJDL(this, (task_->userSpec()), JDLFileName);
  if(jdl_->save(FileSys::oDir())) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
int NewOrcaLocJob::setUniqSuffix() {
  const string sS =(task_->userSpec())->read("OutFileSuffix"); //optional dont care if does not exist
  ostringstream os;
  os<< "." << sS << "_J" << Id();
  uniqSuffix_ = os.str();
  if(Log::level()>2) cout <<"NewOrcaLocJob::setUniqSuffix() Setting uniqSuffix to " << uniqSuffix_<<endl;
  return EXIT_SUCCESS;
}
int NewOrcaLocJob::setStdOutFile() {
  ostringstream os;
  const string sS = ((task_->userSpec())->read("StdOutput")); 
  if(sS.empty())
    os << "stdOut";
  os<< sS <<uniqSuffix();
  stdOutFile_ = os.str();
  if(Log::level()>2) cout <<"NewOrcaLocJob::setStdOutFile() set to "<<stdOutFile_<<endl;
  return EXIT_SUCCESS;
}
int NewOrcaLocJob::setStdErrFile() {
  ostringstream os;
  const string sS = ((task_->userSpec())->read("StdError")); 
  if(sS.empty())
    os<<"stdErr";
  os << sS << uniqSuffix();
  stdErrFile_ = os.str();
  if(Log::level()>2) cout <<"NewOrcaLocJob::setStdErrFile() set to "<<stdErrFile_<<endl;
  return EXIT_SUCCESS;
}
int NewOrcaLocJob::setExecutable() {
  executable_ = "Undefined";
  if(wrapper_)
    executable_ = wrapper_->execNameFullHandle();
  return EXIT_SUCCESS;
}
int NewOrcaLocJob::setLocalInFiles(){
  //Set Local Input files
  vector<string> vS;
  (task_->userSpec())->read("OtherInputFiles", vS); //No need to check if defined or not - optional parameter
  for(vector<string>::const_iterator it = vS.begin(); it!=vS.end(); it++) {
    if(Log::level()>2) cout << "NewOrcaLocJob::setLocalInFiles() reading input file name:" << (*it) << endl;
    vLocalInFiles_.insert(new File(*it));
  }    
  //don't forget user executable is local infile too!
  //const string sF = (task_->userSpec())->read("Executable");  
  //if(sF.empty()) {
  //  cerr <<"NewOrcaLocJob::setLocalInFiles() Error Executable name not defined in JDL"<<endl;
  //  return EXIT_FAILURE;
  //}
  //vLocalInFiles_.insert(new File(sF));

  return EXIT_SUCCESS;
}
int NewOrcaLocJob::setOutFiles(){
  //do not add StdOut or StdErr, BOSS created them in correct dir - wrapper ignore them
  //Now add user definned other files
  vector<string> sVS;
  (task_->userSpec())->read("OutFiles", sVS); //Optional parameter - no need to ensure it's been defined
  for(vector<string>::const_iterator it = sVS.begin(); it!=sVS.end(); it++) {
    if(Log::level()>2) cout << "NewOrcaLocJob::setOutputFiles() reading output file name:" << (*it) << endl;
    ostringstream newfilename;
    newfilename << (*it);
    if(Log::level()>2) cout << "NewOrcaLocJob::setOutputFiles() setting output File name to:" << newfilename.str() << endl;
    vOutFiles_.insert(newfilename.str());
  }
  return EXIT_SUCCESS;
}
int NewOrcaLocJob::setInFiles(){
  //Set Input Data Files from Phys Cat
  ostringstream myQuery;
  myQuery<<"runid=";
  for (int i=0;i<dataSelect_.size();i++) {
    if (i<(dataSelect_.size()-1)) myQuery<<dataSelect()[i]<<" OR runid=";
    else myQuery<<dataSelect_[i];
  }
  if(!myNewOrcaTask_->physCat()) {
    cerr<<"NewOrcaLocJob::setInFiles Error! Task does not have PhysCat set!"<<endl;
    return EXIT_FAILURE;
  }
  const vector<string> myFiles = (myNewOrcaTask_->physCat())->listLFNs(myQuery.str());
  for(vector<string>::const_iterator i = myFiles.begin(); i!= myFiles.end() ; i++){
    if(Log::level()>2) cout<<"NewOrcaLocJob::setInFiles() setting inFile to " << (*i)<<endl;
    vInFiles_.insert(*i);
  }

  //Set Input Meta Data File from Phys Cat into input files
  string query("DataType='META'");
  const vector<string> tmp = (myNewOrcaTask_->physCat())->listLFNs(query);
  if(tmp.size()==0) {
    cerr << "NewOrcaLocJob::setInFiles Error: No Meta Data File found in catalog!" << endl;
    return EXIT_FAILURE;
  }
  for(vector<string>::const_iterator it = tmp.begin(); it!=tmp.end(); ++it) 
    vInFiles_.insert(*it);

  return EXIT_SUCCESS;
}

int NewOrcaLocJob::setOutDir() {
  string sS = ((task_->userSpec())->read("OutDir"));
  if (sS.empty()) {
    cerr << "NewOrcaLocJob::setoutDir Error: No output directory defined"<<endl;
      return EXIT_FAILURE;
  }
  outDir_ = sS;
  return EXIT_SUCCESS;
}
