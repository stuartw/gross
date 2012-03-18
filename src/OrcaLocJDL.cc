#include "OrcaLocJDL.hh"

#include "CladLookup.hh"
#include "OrcaLocJob.hh"
#include "File.hh"

OrcaLocJDL::OrcaLocJDL(const OrcaLocJob* myJob, CladLookup* myUserSpec, const string myFileName) 
  : JDL(myFileName), job_(myJob), userSpec_(myUserSpec) {};

const string OrcaLocJDL::script(){
  ostringstream os;
  
  os << "Executable = \""    << job_->wrapperName()    << "\";"<<endl;
  os << "StdOutput  = \""    << /*job_->outDir() << "/" <<*/ job_->stdOutFile()    << "\";"<<endl;
  os << "StdError   = \""    << /*job_->outDir() << "/" <<*/ job_->stdErrFile()    << "\";"<<endl;
  os << "Arguments  = \""    << job_->outDir()        << "\";" <<endl;

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
     os2 << "\"" << job_->stdOutFile() << "\", \"" << job_->stdErrFile() << "\"";
     for (set<string>::const_iterator i=(job_->outFiles()).begin(); i!= (job_->outFiles()).end(); i++){
       os2 << ", \"" << (*i) << "\"";
     }
     os << "OutputSandbox = {" << os2.str() << /*", \"OutputFiles.txt\"" <<*/ "};" <<endl;
   }

  
  //Any remaining info in JDL passed transparently through
  //Not sure if this is wanted for pbs/lsf etc?
  os << userSpec_->jdlDump();

  return os.str();
}


