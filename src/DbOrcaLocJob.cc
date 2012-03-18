#include "DbOrcaLocJob.hh"

#include "TaskFactory.hh"
#include "Wrapper.hh"
#include "JDL.hh"
#include "LocalDb.hh"
#include "FileSys.hh"
#include "Task.hh"
#include "File.hh"
#include "CladLookup.hh"
#include "ContPrint.hh"
#include "pstream.h"
#include "StringSpecial.hh"
#include "PhysCat.hh"
#include "DbOrcaTask.hh"

using namespace stringSpecial;


DbOrcaLocJob::DbOrcaLocJob(const int anId, const vector<int> aDataSelect, Task* aTask) : 
  OrcaLocJob(anId, aDataSelect, aTask) {

  unInit_=true;
  
  //The order matters here:
  if(setExecutable()) return;
  if(setUniqSuffix()) return;
  if(setLocalInFiles()) return;
  //if(setInFiles()) return;
  if(setSites()) return;
  if(setStdOutFile()) return;
  if(setStdErrFile()) return;
  if(setOutFiles()) return;
  //if(setOutDir()) return;

  unInit_=false;
}

int DbOrcaLocJob::save() {  
  if(OrcaLocJob::save()) return EXIT_FAILURE;

  //tarFile
  if(setOrcaVers()) return EXIT_FAILURE;
  if(setExecutable()) return EXIT_FAILURE;
  if(!setTarFile()) return EXIT_FAILURE;
  
  return EXIT_SUCCESS;             
}

int DbOrcaLocJob::makeSubFiles() {
  if(createOrcaFiles()) return EXIT_FAILURE;
  if(createWrapper()) return EXIT_FAILURE;
  if(createJDL()) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
int DbOrcaLocJob::delSubFiles() {
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
int DbOrcaLocJob::createOrcaFiles() {
  if(setOrcaVers()) {
    cerr << "DbOrcaLocJob::setOrcaVers Error - unable to determine required ORCA version" << endl;
    return EXIT_FAILURE;
  }

  if(!setTarFile()) {
    cerr << "DbOrcaLocJob::setTarFile Error - unable to create tar of executable and libraries" <<endl;
    return EXIT_FAILURE;
  }
  vLocalWrapFiles_.insert(pTarFile_);  
  
  return EXIT_SUCCESS;
}
File* DbOrcaLocJob::setTarFile() {
  if(pTarFile_) return pTarFile_;
  ostringstream tarName;
  tarName << FileSys::oDir() << "/" << executable_ << "_" << orcaVers_  << "_" << getpid() << string(".tar.gz");
  if(Log::level()>2) cout << "DbOrcaLocJob::SetTarFile() Creating Tar file " << tarName.str() << endl;
  pTarFile_ = new File(tarName.str());
  if (pTarFile_->exists()) return pTarFile_;
  //start finding exe and libs
  string output;
  string ARCH;
  string ArchCommand = "scram arch";
  redi::ipstream psub3(ArchCommand, redi::pstreambuf::pstdout|redi::pstreambuf::pstderr);
  while(std::getline(psub3, output)) {
     if(stringSearch(output,"error")) {
      cerr << "DbOrcaLocJob::SetTarFile Error unable to determine SCRAM arch " << output << endl;
      return 0;
    }
    ARCH = output;
  }
  psub3.close();
  if(Log::level()>2) cout << "DbOrcaLocJob::setTarFile() Found SCRAM arch " << ARCH<<endl;
   char* pc=getenv("LOCALRT");
   if (!pc) {
     cerr << "DbOrcaGJob::setTarFile() error ORCA not found - please do eval `scram runtime -sh`" <<endl;
     return 0;
   }  
  string LocalRT(pc);
  if (ARCH==""||LocalRT=="") {
    cerr << "DbOrcaLocJob::setTarFile() error ORCA not found - please do eval `scram runtime -sh`" <<endl;
    return 0;
  }
  string exeName = LocalRT + string("/bin/") + ARCH + string("/") + executable_;
  string libCommand = string("ldd ") + exeName;
  string cdCommand1 = string("cd ") + LocalRT + string(";");
  string working = FileSys::workingDir();
  string cdCommand2 = string("cd ") + working + string(";");
  redi::ipstream psub1(libCommand, redi::pstreambuf::pstdout|redi::pstreambuf::pstderr);
  string libs;
  if(psub1.eof()) {
    cerr << "DbOrcaLocJob::SetTarFile Error unable to locate executable: " << exeName << endl;
    return 0;
  }
  while(std::getline(psub1, output)) {
    if(stringSearch(output,"not found")) {
      cerr << "DbOrcaLocJob::SetTarFile Error unable to locate all libraries: " << output << endl;
      return 0;
    }
    if(stringSearch(output,LocalRT)) {
      string sTemp=output.substr(1,output.find("=>")-2);
      if(Log::level()>2) cout << "DbOrcaLocJob::setTarFile() Found user library " << sTemp <<endl;
      libs = libs+string(" ") + string("lib/") + ARCH + string ("/") + sTemp;
    }
  }
  psub1.close();
  output.clear();
  string tarCommand = cdCommand1 +string("tar zcf ") + tarFile()->fullHandle() + libs + string(" bin/") + ARCH + string("/") +executable_+ string("; ") + cdCommand2;
  redi::ipstream psub2(tarCommand, redi::pstreambuf::pstdout|redi::pstreambuf::pstderr);
  while(std::getline(psub2, output)) {
    if(stringSearch(output,"error")) {
      cerr<<"DbOrcaLocJob::DbTarFile An error has occurred in the command "<<tarCommand <<" " << output<<endl;
      return 0;
    }
  }
  psub2.close();
  return pTarFile_;
}  
int DbOrcaLocJob::setOrcaVers() {
  if (orcaVers_!="") return EXIT_SUCCESS;

  vector<string> myResults;
  ostringstream os;
  os << "TaskID="<<task_->Id()<<"&&JobId="<<Id();
  LocalDb::instance()->tableRead("Analy_Job", "orcaVers", os.str(), myResults);
  if(myResults.size()!=1) return 0;
  string LocalRT = myResults[0];
  if (LocalRT=="") {
    cerr << "DbOrcaLocJob::setOrcaVers Error! Scram version not set in database" << endl;
    return EXIT_FAILURE;
  }
  orcaVers_ = LocalRT;
  if (Log::level()>2) cout << "DbOrcaLocJob::setOrcaVers retrieved ORCA version: " << orcaVers_<<endl;
  return EXIT_SUCCESS;
}
int DbOrcaLocJob::createWrapper(){ //Always created "on the fly" (i.e. not persistent)
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
int DbOrcaLocJob::createJDL(){ //Always created "on the fly" (i.e. not persistent)
  string JDLFileName=string("orca.jdl") + uniqSuffix();
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
/*int DbOrcaLocJob::setInFiles(){
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
}*/
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
int DbOrcaLocJob::setSites() {
  if(Log::level()>2) cout << "DbOrcaLocJob::setSites() finding PubDBs with this dataset" << endl;
  const DbOrcaTask* mytask_ = dynamic_cast<const DbOrcaTask*> (task());
  vSites = mytask_->physCat()->getSites4MyRuns(dataSelect());
  vector<string> PoolCatFiles;
  task_->userSpec()->read("ExtraPoolCatalogs", PoolCatFiles);
  if(Log::level()>2&&!PoolCatFiles.empty()) cout << "DbOrcaLocJob::setSites() using POOL catalog " << PoolCatFiles << endl;
  if (vSites.empty()&&PoolCatFiles.empty()) {
    cerr << "DbOrcaLocJob::setSites() Error No PubDb's found with this dataset and no POOL catalogs specified" << endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
