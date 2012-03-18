#include "OrcaLocJDL.hh"

#include "CladLookup.hh"
#include "OrcaLocJob.hh"
#include "File.hh"

OrcaLocJDL::OrcaLocJDL(const OrcaLocJob* myJob, const CladLookup* myUserSpec, const string myFileName) 
  : JDL(myFileName), job_(myJob), userSpec_(myUserSpec) {};

const string OrcaLocJDL::script(){
  ostringstream os;
  
  os << "Executable = \""    << job_->executable()    << "\";"<<endl;
  os << "StdOutput  = \""    << job_->outDir() << "/" << job_->stdOutFile()    << "\";"<<endl;
  os << "StdError   = \""    << job_->outDir() << "/" << job_->stdErrFile()    << "\";"<<endl;
  os << "Arguments  = \""    << job_->outDir()        << "\";" <<endl;

  //Any remaining info in JDL passed transparently through
  //Not sure if this is wanted for pbs etc?
  os << userSpec_->jdlDump();

  return os.str();
}


