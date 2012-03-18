#include "DbJob.hh"

#include <BossCommandInterpreter.h>
#include "Wrapper.hh"
#include "JDL.hh"
#include "LocalDb.hh"
#include "FileSys.hh"
#include "Task.hh"
#include "File.hh"
#include "CladLookup.hh"

//First explicitly inherit from Job base class
int DbJob::save() {return Job::save();}; //Not really necessary, as read from Db
int DbJob::clean() {return Job::clean();};

//Member functions
DbJob::DbJob(const int anId, const int aDataSelect, Task* aTask) : Job(anId, aDataSelect, aTask), pXMLFragFile_(0) {};

DbJob::~DbJob() {
  if(pXMLFragFile_) delete pXMLFragFile_;
}

int DbJob::init(){
  //The order matters here:
  if(this->setUniqSuffix()) return EXIT_FAILURE;
  if(this->setLocalInFiles()) return EXIT_FAILURE;
  if(this->setInGUIDs()) return EXIT_FAILURE;
  if(this->setStdOutFile()) return EXIT_FAILURE;
  if(this->setStdErrFile()) return EXIT_FAILURE;
  if(this->setOutSandboxFiles()) return EXIT_FAILURE;
  if(this->setOutGUIDs()) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
int DbJob::createSubFiles(string myType) {
  if(this->createOrcaFiles()) return EXIT_FAILURE;
  if(this->createWrapper(myType)) return EXIT_FAILURE;
  if(this->createJDL()) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
int DbJob::createOrcaFiles() {
  //Orca XML File
  if(!this->setXMLFrag()) {
    cerr<<"DbJob::createOrcaFiles Error - unable to create XML Fragment for this run"<<endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
File* DbJob::setXMLFrag() {
  if(pXMLFragFile_) return pXMLFragFile_;

  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<this->Id();  
  LocalDb::instance()->tableRead("Analy_Job", "XMLFrag", os.str(), myResults);
  if(myResults.size()!=1) return 0;
  string contents=myResults[0];
  
  if(contents.empty()) {
    cerr<<"DbJob::setXMLFrag() Error XML File contents empty in database"<<endl;
    return 0;
  }
  
  string xMLFragName = FileSys::oDir() + string("/") + string("XMLFrag.xml")+ this->uniqSuffix();
  if(Log::level()>2) cout <<"DbJob::setXMLFrag() Creating XMLFrag from DB with name " << xMLFragName<<endl;
  pXMLFragFile_ = new File(xMLFragName);
  pXMLFragFile_->save(contents);
  return pXMLFragFile_;
}

int DbJob::createWrapper(string myType){ //Always created "on the fly" (i.e. not persistent)
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
int DbJob::createJDL(){ //Always created "on the fly" (i.e. not persistent)
  string JDLFileName=string("analy.jdl") + this->uniqSuffix();
  if(!jdl_) jdl_ = new JDL(this, (task_->userSpec()), JDLFileName);
  if(jdl_->save(FileSys::oDir())) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
int DbJob::setExecutable() {
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<this->Id();
  
  LocalDb::instance()->tableRead("Analy_Job", "ExecName", os.str(), myResults);
  if(myResults.size()!=1) return EXIT_FAILURE;  
  if(!myResults[0].empty()) executable_=myResults[0];  
  else {
    cerr<<"DbJob::setExecutable Error - not set in DB"<<endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
int DbJob::setUniqSuffix() {
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<this->Id();
  
  LocalDb::instance()->tableRead("Analy_Job", "Suffix", os.str(), myResults);
  if(myResults.size()!=1) return EXIT_FAILURE;
  
  if(!myResults[0].empty()) uniqSuffix_=myResults[0];
  else {
    cerr<<"DbJob::setUniqSuffix Error - not set in DB"<<endl;
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
int DbJob::setStdOutFile() {
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<this->Id();
  
  LocalDb::instance()->tableRead("Analy_Job", "stdOut", os.str(), myResults);
  if(myResults.size()!=1) return EXIT_FAILURE;

  if(!myResults[0].empty()) stdOutFile_=myResults[0];
  else {
    cerr<<"DbJob::setStdOutFile Error - not set in DB"<<endl;
    return EXIT_FAILURE;
  }

  
  return EXIT_SUCCESS;
}
int DbJob::setStdErrFile() {
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<this->Id();
  
  LocalDb::instance()->tableRead("Analy_Job", "stdErr", os.str(), myResults);
  if(myResults.size()!=1) return EXIT_FAILURE;

  if(!myResults[0].empty()) stdErrFile_=myResults[0]; 
  else {
    cerr<<"DbJob::setStdErrFile Error - not set in DB"<<endl;
    return EXIT_FAILURE;
  }
 
  return EXIT_SUCCESS;
}
int DbJob::setInGUIDs(){
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<this->Id();

  if(LocalDb::instance()->tableRead("Analy_InGUIDs", "Name", os.str(), myResults)|| 
     !(myResults.size())) return EXIT_FAILURE;

  for(vector<string>::const_iterator i = myResults.begin(); i!= myResults.end() ; i++) {
    if(!(*i).empty()) vInGUIDs_.push_back(*i);  
    else {
      cerr<<"DbJob::setInGUIDs Error - not set in DB"<<endl;
      return EXIT_FAILURE;
    }
    if(Log::level()>2) cout <<"DbJob::setInGUIDs() Read inGUID name "<<(*i) <<endl;
  }

  //Meta Data File
  myResults.clear();
  
  LocalDb::instance()->tableRead("Analy_Job", "MetaFile", os.str(), myResults);
  if(myResults.size()!=1) return EXIT_FAILURE;

  if(!myResults[0].empty()) metaFile_=myResults[0]; 
  else {
    cerr<<"DbJob::setInGUIDs Error - metaFile not set in DB"<<endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
int DbJob::setLocalInFiles(){
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<this->Id();

  if(LocalDb::instance()->tableRead("Analy_LocalIn", "Name", os.str(), myResults)|| 
     !(myResults.size())) return EXIT_FAILURE;

  for(vector<string>::const_iterator i = myResults.begin(); i!= myResults.end() ; i++) {
    if(!(*i).empty()) vLocalInFiles_.push_back(new File(*i));
    else {
      cerr<<"DbJob::setLocalInFiles Error - not set in DB"<<endl;
      return EXIT_FAILURE;
    }
    if(Log::level()>2) cout <<"DbJob::setLocalInFiles() Reading in LocalFile name "<< (*i) <<endl;
  }
  return EXIT_SUCCESS;
}
int DbJob::setOutSandboxFiles(){
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<this->Id();
  
  if(LocalDb::instance()->tableRead("Analy_OutSandbox", "Name", os.str(), myResults)|| 
     !(myResults.size())) return EXIT_FAILURE;
  
  for(vector<string>::const_iterator i = myResults.begin(); i!= myResults.end() ; i++) {
    if(!(*i).empty()) vOutSandboxFiles_.push_back(*i);  
    else {
      cerr<<"DbJob::setOutSandboxFiles Error - not set in DB"<<endl;
      return EXIT_FAILURE;
    }
    if(Log::level()>2) cout <<"DbJob::setOutSandboxFiles() Reading SandBoxFile name "<<(*i) <<endl;
  }
  return EXIT_SUCCESS;
}
int DbJob::setOutGUIDs(){
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<this->Id();
  
  if(LocalDb::instance()->tableRead("Analy_OutGUIDs", "Name", os.str(), myResults)|| 
     !(myResults.size())) return EXIT_SUCCESS; //Not necessarily a failure
  
  for(vector<string>::const_iterator i = myResults.begin(); i!= myResults.end() ; i++) {
    if(!(*i).empty()) vOutGUIDs_.push_back(*i);  
    else {
      //cerr<<"DbJob::setOutGUIDs Error - not set in DB"<<endl;
      return EXIT_SUCCESS; //Not necessarily a failure
    }
    if(Log::level()>2) cout <<"DbJob::setOutGUIDs() Reading OutGUID name "<<(*i) <<endl;
  }
  return EXIT_SUCCESS;
}
