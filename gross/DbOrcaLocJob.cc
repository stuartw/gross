#include "DbOrcaLocJob.hh"

#include "TaskFactory.hh"
#include "Wrapper.hh"
#include "JDL.hh"
#include "LocalDb.hh"
#include "FileSys.hh"
#include "Task.hh"
#include "File.hh"
#include "CladLookup.hh"


DbOrcaLocJob::DbOrcaLocJob(const int anId, const vector<int> aDataSelect, Task* aTask) : 
  OrcaLocJob(anId, aDataSelect, aTask) {

  unInit_=true;
  
  //The order matters here:
  if(setUniqSuffix()) return;
  if(setLocalInFiles()) return;
  if(setInFiles()) return;
  if(setStdOutFile()) return;
  if(setStdErrFile()) return;
  if(setOutFiles()) return;
  if(setOutDir()) return;

  unInit_=false;
}

int DbOrcaLocJob::save() {  
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

int DbOrcaLocJob::makeSubFiles() {
  if(createOrcaFiles()) return EXIT_FAILURE;
  if(createWrapper()) return EXIT_FAILURE;
  if(createJDL()) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
int DbOrcaLocJob::createOrcaFiles() {
  //Orca XML File
  if(!setXMLFrag()) {
    cerr<<"DbOrcaLocJob::createOrcaFiles Error - unable to create XML Fragment for this run"<<endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
File* DbOrcaLocJob::setXMLFrag() {
  if(pXMLFragFile_) return pXMLFragFile_;

  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<Id();  
  LocalDb::instance()->tableRead("Analy_Job", "XMLFrag", os.str(), myResults);
  if(myResults.size()!=1) return 0;
  string contents=myResults[0];
  
  if(contents.empty()) {
    cerr<<"DbOrcaLocJob::setXMLFrag() Error XML File contents empty in database"<<endl;
    return 0;
  }
  
  string xMLFragName = FileSys::oDir() + string("/") + string("XMLFrag.xml")+ uniqSuffix();
  if(Log::level()>2) cout <<"DbOrcaLocJob::setXMLFrag() Creating XMLFrag from DB with name " << xMLFragName<<endl;
  pXMLFragFile_ = new File(xMLFragName);
  pXMLFragFile_->save(contents);
  return pXMLFragFile_;
}

int DbOrcaLocJob::createWrapper(){ //Always created "on the fly" (i.e. not persistent)
  string wrapFileName=string("wrapper.sh")+ uniqSuffix();
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
int DbOrcaLocJob::createJDL(){ //Always created "on the fly" (i.e. not persistent)
  string JDLFileName=string("submit.jdl") + uniqSuffix();
  if(!jdl_) jdl_ = (TaskFactory::instance())->makeJDL(this, (task_->userSpec()), JDLFileName);
  if(jdl_->save(FileSys::oDir())) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
int DbOrcaLocJob::setExecutable() {
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<Id();
  
  LocalDb::instance()->tableRead("Analy_Job", "ExecName", os.str(), myResults);
  if(myResults.size()!=1) return EXIT_FAILURE;  
  if(!myResults[0].empty()) executable_=myResults[0];  
  else {
    cerr<<"DbOrcaLocJob::setExecutable Error - not set in DB"<<endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
int DbOrcaLocJob::setUniqSuffix() {
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<Id();
  
  LocalDb::instance()->tableRead("Analy_Job", "Suffix", os.str(), myResults);
  if(myResults.size()!=1) return EXIT_FAILURE;
  
  if(!myResults[0].empty()) uniqSuffix_=myResults[0];
  else {
    cerr<<"DbOrcaLocJob::setUniqSuffix Error - not set in DB"<<endl;
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
int DbOrcaLocJob::setStdOutFile() {
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<Id();
  
  LocalDb::instance()->tableRead("Analy_Job", "stdOut", os.str(), myResults);
  if(myResults.size()!=1) return EXIT_FAILURE;

  if(!myResults[0].empty()) stdOutFile_=myResults[0];
  else {
    cerr<<"DbOrcaLocJob::setStdOutFile Error - not set in DB"<<endl;
    return EXIT_FAILURE;
  }

  
  return EXIT_SUCCESS;
}
int DbOrcaLocJob::setStdErrFile() {
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<Id();
  
  LocalDb::instance()->tableRead("Analy_Job", "stdErr", os.str(), myResults);
  if(myResults.size()!=1) return EXIT_FAILURE;

  if(!myResults[0].empty()) stdErrFile_=myResults[0]; 
  else {
    cerr<<"DbOrcaLocJob::setStdErrFile Error - not set in DB"<<endl;
    return EXIT_FAILURE;
  }
 
  return EXIT_SUCCESS;
}
int DbOrcaLocJob::setInFiles(){
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<Id();

  if(LocalDb::instance()->tableRead("AnalyLoc_InFiles", "Name", os.str(), myResults)|| 
     !(myResults.size())) return EXIT_FAILURE;

  for(vector<string>::const_iterator i = myResults.begin(); i!= myResults.end() ; i++) {
    if(!(*i).empty()) vInFiles_.insert(*i);  
    else {
      cerr<<"DbOrcaLocJob::setInFiles Error - not set in DB"<<endl;
      return EXIT_FAILURE;
    }
    if(Log::level()>2) cout <<"DbOrcaGJob::setInFiles() Read in name "<<(*i) <<endl;
  }

  //Meta Data File
  //For local jobs these are currently just another InFile
  //In later release maybe have metaFile listed to get to zipped Metadata

  return EXIT_SUCCESS;
}
int DbOrcaLocJob::setLocalInFiles(){
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<Id();

  if(LocalDb::instance()->tableRead("Analy_LocalIn", "Name", os.str(), myResults)|| 
     !(myResults.size())) return EXIT_FAILURE;

  for(vector<string>::const_iterator i = myResults.begin(); i!= myResults.end() ; i++) {
    if(!(*i).empty()) vLocalInFiles_.insert(new File(*i));
    else {
      cerr<<"DbOrcaLocJob::setLocalInFiles Error - not set in DB"<<endl;
      return EXIT_FAILURE;
    }
    if(Log::level()>2) cout <<"DbOrcaLocJob::setLocalInFiles() Reading in LocalFile name "<< (*i) <<endl;
  }
  return EXIT_SUCCESS;
}
int DbOrcaLocJob::setOutFiles(){
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<Id();
  
  if(LocalDb::instance()->tableRead("AnalyLoc_OutFiles", "Name", os.str(), myResults)|| 
     !(myResults.size())) return EXIT_SUCCESS; //Not necessarily a failure
  
  for(vector<string>::const_iterator i = myResults.begin(); i!= myResults.end() ; i++) {
    if(!(*i).empty()) vOutFiles_.insert(*i);  
    else {
      //cerr<<"DbOrcaGJob::setOutGUIDs Error - not set in DB"<<endl;
      return EXIT_SUCCESS; //Not necessarily a failure
    }
    if(Log::level()>2) cout <<"DbOrcaLocJob::setOutFiles() Reading Outfile name "<<(*i) <<endl;
  }
  return EXIT_SUCCESS;
}

int DbOrcaLocJob::setOutDir() {
  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobID="<<Id();
  
  LocalDb::instance()->tableRead("Analy_Job", "outDir", os.str(), myResults);
  if(myResults.size()!=1) return EXIT_FAILURE;  
  if(!myResults[0].empty()) outDir_=myResults[0];  
  else {
    cerr<<"DbOrcaLocJob::setOutDir Error - not set in DB"<<endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
