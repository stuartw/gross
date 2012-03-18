#include "OrcaJDL.hh"

#include "CladLookup.hh"
#include "OrcaGJob.hh"
#include "File.hh"

OrcaJDL::OrcaJDL(const OrcaGJob* myJob, const CladLookup* myUserSpec, const string myFileName) 
  : JDL(myFileName), job_(myJob), userSpec_(myUserSpec) {};

const string OrcaJDL::script(){
  ostringstream os;
  
  os << "Executable = \""    << job_->executable()    << "\";"<<endl;
  os << "StdOutput  = \""    << job_->stdOutFile()    << "\";"<<endl;
  os << "StdError   = \""    << job_->stdErrFile()    << "\";"<<endl;

  
  {//InputSandbox
    ostringstream os2;
    int first=1;
    //LocalInFiles
    for (set<File*>::const_iterator i=(job_->localInFiles()).begin(); i!= (job_->localInFiles()).end(); i++){
      if(first) {first=0;}
      else {os2 << " , ";}
      os2 << "\"" << (*i)->fullHandle() << "\"";
    }  
    //And don't forget the wrap files 
    for (set<File*>::const_iterator i=(job_->localWrapFiles()).begin(); i!= (job_->localWrapFiles()).end(); i++){
      if(first) {first=0;}
      else {os2 << " , ";}
      os2 << "\"" << (*i)->fullHandle() << "\"";
    }    
    os << "InputSandbox = {" << os2.str() << "};" <<endl;
  }  
  {//OutputSandbox
    ostringstream os2;
    int first=1;
    for (set<string>::const_iterator i=(job_->outSandboxFiles()).begin(); i!= (job_->outSandboxFiles()).end(); i++){
      if(first) {first=0;}
      else {os2 << " , ";}
      os2 << "\"" << (*i) << "\"";
    }
    os << "OutputSandbox = {" << os2.str() << "};" <<endl;
  }
  {//InGUIDs
    ostringstream os2;
    int first=1;
    for (set<string>::const_iterator i=(job_->inGUIDs()).begin(); i!= (job_->inGUIDs()).end(); i++){
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


