#include "OrcaJDL.hh"

#include "CladLookup.hh"
#include "OrcaGJob.hh"
#include "File.hh"
#include "Site.hh"

OrcaJDL::OrcaJDL(const OrcaGJob* myJob, CladLookup* myUserSpec, const string myFileName) 
  : JDL(myFileName), job_(myJob), userSpec_(myUserSpec) {};

const string OrcaJDL::script(){
  ostringstream os;
  
  os << "Executable = \""    << job_->wrapperName()   << "\";"<<endl;
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
    os << "OutputSandbox = {" << os2.str() << /*", \"OutputFiles.txt\"" <<*/ "};" <<endl;
  }
  //No longer put input data in JDL
  //may in the future use clarens RB data location service
  //but will need way for job to contact PubDb's
  //Currently restrict sites for match making at prepare ntime from PubDBs
  /*{//InGUIDs
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
  //gridftp is basic requirement of all SE's
  os << "DataAccessProtocol= {\"gsiftp\"};"<<endl;
  */
  {//OutputLFNs  - Now handled by wrapper script - BOSS does not understand line
    /*os << "OutputData = {";
    for (set<string>::const_iterator i=(job_->outGUIDs()).begin(); i!= (job_->outGUIDs()).end(); i++)  {
      if (i!=job_->outGUIDs().begin()) os << ",";
      os << "[";
      os << "OutputFile=\"" << *i << "\";";
      os << "LogicalFileName=\"lfn:" << *i << job_->uniqSuffix() << "\";";
      os << "]";
    }
    os << "};" << endl;*/
  }  
  //Sites with PubDBs containning dataset
  ostringstream requirements;
  requirements << userSpec_->read("Requirements");
  if (requirements.str()!="") requirements << " && ";
  requirements << "Member(\"VO-cms-" << job_->orcaVers() << "\", other.GlueHostApplicationSoftwareRunTimeEnvironment)";
  vector<Site*> sites = job_->sites();
  requirements << " && (";
  for (vector<Site*>::const_iterator i=sites.begin(); i!=sites.end(); i++) {
    vector<string> CEs = (*i)->getCE();
    for (vector<string>::const_iterator j=CEs.begin(); j!=CEs.end(); j++) {
      if (i!=sites.begin()) requirements << " || ";
      requirements << "other.GlueCEInfoHostName == \"" << (*j) << "\"";
    }
  }
  requirements << ")";
  os << "Requirements = " << requirements.str() << ";" << endl;   
  
  //Any remaining info in JDL passed transparently through
    os << userSpec_->jdlDump();
  
  return os.str();
}


