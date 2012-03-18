#include "EDGIf.hh"

#include "pstream.h"
#include "StringSpecial.hh"

using namespace stringSpecial;

string EDGIf::getOutput(string jobId, string outDir/*="./"*/) const {
  string retDir("");
  if(jobId.empty()) return retDir;

  std::ostringstream command;
  command << "edg-job-get-output " << " --dir " << outDir << " " << jobId;
  if(Log::level()>2) cout <<"EDGIf::getOutput command is "<<command.str()<<endl;
  redi::ipstream psub(command.str(), redi::pstreambuf::pstdout|redi::pstreambuf::pstderr);
  string output;
  //Parse output of command
  while(std::getline(psub, output)) {
    //Check for error at any stage
    if(stringSearch(output,"error")) {
      cerr<<"EDGIf::getOutput An error has occurred in the command "<<command.str() <<" " << output<<endl;
      return "";
    }	
    //Success criterion
    if(stringSearch(output,"successfully retrieved and stored in the directory:"))
      std::getline(psub, retDir);    
  }
  if(Log::level()>2) cout <<"EDGIf::getOutput return sbox dir"<<retDir<<endl;
  return retDir;  
}
