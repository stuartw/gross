#include "EDGIf.hh"

#include "pstream.h"
#include "StringSpecial.hh"

using namespace stringSpecial;

string EDGIf::getOutput(int jobId, string outDir/*="./"*/) const {
  if(jobId==0) return "";

  std::ostringstream command;
  //command << "edg-job-get-output " << " --dir " << outDir << " " << jobId;
  command << "boss-edg-job-get-output " << jobId;
  if(Log::level()>2) cout <<"EDGIf::getOutput command is "<<command.str()<<endl;
  redi::ipstream psub(command.str(), redi::pstreambuf::pstdout|redi::pstreambuf::pstderr);
  std::string output;
  //Parse output of command
  while(std::getline(psub, output)) {
    //Check for error at any stage
    if(stringSearch(output,"error")) {
      cerr<<"EDGIf::getOutput An error has occurred in the command "<<command.str() <<" " << output<<endl;
      return "";
    }  else if(stringSearch(output,"have been retrieved")) {
         if(Log::level()>2) cout <<"EDGIf::getOutput sandbox saved to dir "<<outDir<<endl;
	 std::ostringstream sbox;
	 sbox << outDir << "/BossJob_" << jobId;
	 return sbox.str();
    }
  }
  return "";
}
string EDGIf::untarLocalOutput(const int bossId,const string oDir) const {
  //function to untar output files from local jobs
  if(bossId==0) return "";
  std::ostringstream command;
  command << "mkdir -p " << oDir << "/BossJob_" << bossId;
  if(system(command.str().c_str())) {
    cerr << "EDGIf::untarLocalOutput() An error occured in the command " << command.str() <<endl;
    return "";
  }
  command.str("");
  command << "mv BossOutArchive_" << bossId << ".tgz " << oDir << "/BossJob_" << bossId;
  if(system(command.str().c_str())) {
    cerr << "EDGIf::untarLocalOutput() An error occured in the command " << command.str() <<endl;
    return "";
  }
  command.str("");
  command << "cd " << oDir << "/BossJob_" << bossId << "; tar xzf BossOutArchive_" << bossId<<".tgz";
  if(Log::level()>2) cout <<"EDGIf::untarLocalOutput command is "<<command.str()<<endl;
  if(system(command.str().c_str())) {
    cerr << "EDGIf::untarLocalOutput() An error occured in the command " << command.str() <<endl;
    return "";
  }
  std::ostringstream sbox;
  sbox << oDir << "/BossJob_" << bossId;
  return sbox.str();
}

