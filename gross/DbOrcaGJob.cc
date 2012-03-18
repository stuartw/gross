#include "DbOrcaGJob.hh"

#include "TaskFactory.hh"
#include "Wrapper.hh"
#include "JDL.hh"
#include "LocalDb.hh"
#include "FileSys.hh"
#include "Task.hh"
#include "File.hh"
#include "CladLookup.hh"


DbOrcaGJob::DbOrcaGJob(const int anId, const int aDataSelect, Task* aTask) : 
  OrcaGJob(anId, aDataSelect, aTask) {

  unInit_=true;
  
  //The order matters here:
  if(setUniqSuffix()) return;
  if(setLocalInFiles()) return;
  if(setInGUIDs()) return;
  if(setStdOutFile()) return;
  if(setStdErrFile()) return;
  if(setOutSandboxFiles()) return;
  if(setOutGUIDs()) return;

  unInit_=false;
}

int DbOrcaGJob::save() {  
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

int DbOrcaGJob::makeSubFiles() {
  if(createOrcaFiles()) return EXIT_FAILURE;
  if(createWrapper()) return EXIT_FAILURE;
  if(createJDL()) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
int DbOrcaGJob::createOrcaFiles() {
  //Orca XML File
  if(!setXMLFrag()) {
    cerr<<"DbOrcaGJob::createOrcaFiles Error - unable to create XML Fragment for this run"<<endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
File* DbOrcaGJob::setXMLFrag() {
  if(pXMLFragFile_) return pXMLFragFile_;

  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<Id();  
  LocalDb::instance()->tableRead("Analy_Job", "XMLFrag", os.str(), myResults);
  if(myResults.size()!=1) return 0;
  string contents=myResults[0];
  
  if(contents.empty()) {
    cerr<<"DbOrcaGJob::setXMLFrag() Error XML File contents empty in database"<<endl;
    return 0;
  }
  
  string xMLFragName = FileSys::oDir() + string("/") + string("XMLFrag.xml")+ uniqSuffix();
  if(Log::level()>2) cout <<"DbOrcaGJob::setXMLFrag() Creating XMLFrag from DB with name " << xMLFragName<<endl;
  pXMLFragFile_ = new File(xMLFragName);
  pXMLFragFile_->save(contents);
  return pXMLFragFile_;
}

int DbOrcaGJob::createWrapper(){ //Always created "on the fly" (i.e. not persistent)
  string wrapFileName=string("orca.wrapper.sh")+ uniqSuffix();
  if(!wrapper_) wrapper_ = (TaskFactory::instance())->makeWrapper(this, (task_->userSpec()), wrapFileName);
  if(wrapper_->save(FileSys::oDir())) return EXIT_FAILURE;
  //Note that need LocalWrapFiles as well as LocalInFiles because wrap files are created
  //on the fly. So will be created again for DbOrcaGJob. So can't be treated like LocalInFiles which
  //are saved to Db and re-read from Db by DbOrcaGJob else would keep adding more and more entries for wrap files.
  for(set<File*>::const_iterator i=(wrapper_->files()).begin(); i!=(wrapper_->files()).end(); i++)
    vLocalWrapFiles_.insert(*i);
  
  setExecutable(); //Can only be done here, once wrapper has been created.

  return EXIT_SUCCESS;
}
int DbOrcaGJob::createJDL(){ //Always created "on the fly" (i.e. not persistent)
  string JDLFileName=string("analy.jdl") + uniqSuffix();
  if(!jdl_) jdl_ = (TaskFactory::instance())->makeJDL(this, (task_->userSpec()), JDLFileName);
  if(jdl_->save(FileSys::oDir())) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
int DbOrcaGJob::setExecutable() {
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<Id();
  
  LocalDb::instance()->tableRead("Analy_Job", "ExecName", os.str(), myResults);
  if(myResults.size()!=1) return EXIT_FAILURE;  
  if(!myResults[0].empty()) executable_=myResults[0];  
  else {
    cerr<<"DbOrcaGJob::setExecutable Error - not set in DB"<<endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
int DbOrcaGJob::setUniqSuffix() {
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<Id();
  
  LocalDb::instance()->tableRead("Analy_Job", "Suffix", os.str(), myResults);
  if(myResults.size()!=1) return EXIT_FAILURE;
  
  if(!myResults[0].empty()) uniqSuffix_=myResults[0];
  else {
    cerr<<"DbOrcaGJob::setUniqSuffix Error - not set in DB"<<endl;
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
int DbOrcaGJob::setStdOutFile() {
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<Id();
  
  LocalDb::instance()->tableRead("Analy_Job", "stdOut", os.str(), myResults);
  if(myResults.size()!=1) return EXIT_FAILURE;

  if(!myResults[0].empty()) stdOutFile_=myResults[0];
  else {
    cerr<<"DbOrcaGJob::setStdOutFile Error - not set in DB"<<endl;
    return EXIT_FAILURE;
  }

  
  return EXIT_SUCCESS;
}
int DbOrcaGJob::setStdErrFile() {
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<Id();
  
  LocalDb::instance()->tableRead("Analy_Job", "stdErr", os.str(), myResults);
  if(myResults.size()!=1) return EXIT_FAILURE;

  if(!myResults[0].empty()) stdErrFile_=myResults[0]; 
  else {
    cerr<<"DbOrcaGJob::setStdErrFile Error - not set in DB"<<endl;
    return EXIT_FAILURE;
  }
 
  return EXIT_SUCCESS;
}
int DbOrcaGJob::setInGUIDs(){
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<Id();

  if(LocalDb::instance()->tableRead("Analy_InGUIDs", "Name", os.str(), myResults)|| 
     !(myResults.size())) return EXIT_FAILURE;

  for(vector<string>::const_iterator i = myResults.begin(); i!= myResults.end() ; i++) {
    if(!(*i).empty()) vInGUIDs_.insert(*i);  
    else {
      cerr<<"DbOrcaGJob::setInGUIDs Error - not set in DB"<<endl;
      return EXIT_FAILURE;
    }
    if(Log::level()>2) cout <<"DbOrcaGJob::setInGUIDs() Read inGUID name "<<(*i) <<endl;
  }

  //Meta Data File
  myResults.clear();
  
  LocalDb::instance()->tableRead("Analy_Job", "MetaFile", os.str(), myResults);
  if(myResults.size()!=1) return EXIT_FAILURE;

  if(!myResults[0].empty()) metaFile_=myResults[0]; 
  else {
    cerr<<"DbOrcaGJob::setInGUIDs Error - metaFile not set in DB"<<endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
int DbOrcaGJob::setLocalInFiles(){
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<Id();

  if(LocalDb::instance()->tableRead("Analy_LocalIn", "Name", os.str(), myResults)|| 
     !(myResults.size())) return EXIT_FAILURE;

  for(vector<string>::const_iterator i = myResults.begin(); i!= myResults.end() ; i++) {
    if(!(*i).empty()) vLocalInFiles_.insert(new File(*i));
    else {
      cerr<<"DbOrcaGJob::setLocalInFiles Error - not set in DB"<<endl;
      return EXIT_FAILURE;
    }
    if(Log::level()>2) cout <<"DbOrcaGJob::setLocalInFiles() Reading in LocalFile name "<< (*i) <<endl;
  }
  return EXIT_SUCCESS;
}
int DbOrcaGJob::setOutSandboxFiles(){
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<Id();
  
  if(LocalDb::instance()->tableRead("Analy_OutSandbox", "Name", os.str(), myResults)|| 
     !(myResults.size())) return EXIT_FAILURE;
  
  for(vector<string>::const_iterator i = myResults.begin(); i!= myResults.end() ; i++) {
    if(!(*i).empty()) vOutSandboxFiles_.insert(*i);  
    else {
      cerr<<"DbOrcaGJob::setOutSandboxFiles Error - not set in DB"<<endl;
      return EXIT_FAILURE;
    }
    if(Log::level()>2) cout <<"DbOrcaGJob::setOutSandboxFiles() Reading SandBoxFile name "<<(*i) <<endl;
  }
  return EXIT_SUCCESS;
}
int DbOrcaGJob::setOutGUIDs(){
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<Id();
  
  if(LocalDb::instance()->tableRead("Analy_OutGUIDs", "Name", os.str(), myResults)|| 
     !(myResults.size())) return EXIT_SUCCESS; //Not necessarily a failure
  
  for(vector<string>::const_iterator i = myResults.begin(); i!= myResults.end() ; i++) {
    if(!(*i).empty()) vOutGUIDs_.insert(*i);  
    else {
      //cerr<<"DbOrcaGJob::setOutGUIDs Error - not set in DB"<<endl;
      return EXIT_SUCCESS; //Not necessarily a failure
    }
    if(Log::level()>2) cout <<"DbOrcaGJob::setOutGUIDs() Reading OutGUID name "<<(*i) <<endl;
  }
  return EXIT_SUCCESS;
}
