#include "JDL.hh"

#include "CladLookup.hh"
#include "Job.hh"
#include "File.hh"

JDL::JDL(const Job* myJob, const CladLookup* myUserSpec, const string myFileName) 
  : job_(myJob), userSpec_(myUserSpec), name_(myFileName), fullHandle_(myFileName) {
}

const int JDL::save(string myDir) {
  fullHandle_=myDir + "/" + name_;
  if(Log::level()>0) cout << "JDL::save() Saving JDL file " << fullHandle_ <<endl;
  File myFile(fullHandle_);
  if(myFile.save(this->script())) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

const string JDL::script(){
  ostringstream os;
  
  os << "Executable = \""    << job_->executable()    << "\";"<<endl;
  os << "StdOutput  = \""    << job_->stdOutFile()    << "\";"<<endl;
  os << "StdError   = \""    << job_->stdErrFile()    << "\";"<<endl;

  
  {//InputSandbox
    ostringstream os2;
    int first=1;
    //LocalInFiles
    for (vector<File*>::const_iterator i=(job_->localInFiles()).begin(); i!= (job_->localInFiles()).end(); i++){
      if(first) {first=0;}
      else {os2 << " , ";}
      os2 << "\"" << (*i)->fullHandle() << "\"";
    }  
    //And don't forget the wrap files 
    for (vector<File*>::const_iterator i=(job_->localWrapFiles()).begin(); i!= (job_->localWrapFiles()).end(); i++){
      if(first) {first=0;}
      else {os2 << " , ";}
      os2 << "\"" << (*i)->fullHandle() << "\"";
    }    
    os << "InputSandbox = {" << os2.str() << "};" <<endl;
  }  
  {//OutputSandbox
    ostringstream os2;
    int first=1;
    for (vector<string>::const_iterator i=(job_->outSandboxFiles()).begin(); i!= (job_->outSandboxFiles()).end(); i++){
      if(first) {first=0;}
      else {os2 << " , ";}
      os2 << "\"" << (*i) << "\"";
    }
    os << "OutputSandbox = {" << os2.str() << "};" <<endl;
  }
  {//InGUIDs
    ostringstream os2;
    int first=1;
    for (vector<string>::const_iterator i=(job_->inGUIDs()).begin(); i!= (job_->inGUIDs()).end(); i++){
      if(first) {first=0;}
      else {os2 << " , ";}
      os2 << "\"lfn:" << (*i) << "\"";
    }
    os << "InputData = {" << os2.str() << "};" <<endl;
  }
  //In case user has not added this requirement (duplicate entry is ok):
  os << "DataAccessProtocol= {\"file\", \"gridftp\", \"rfio\"};"<<endl;
  //Any remaining info in JDL passed transparently through
  os << userSpec_->jdlDump();

  return os.str();
}


