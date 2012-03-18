#include "Wrapper.hh"

#include "LocalDb.hh"
#include "File.hh"
#include "DeletePtr.hh"
#include <algorithm>

Wrapper::Wrapper(const string myExecName, const string myType) 
  : execName_(myExecName), execNameFullHandle_(myExecName), type_(myType){};

Wrapper::~Wrapper() {
  //delete container of pointers to objects
  for_each(filesCreated_.begin(), filesCreated_.end(), deletePtr<File>);
  filesCreated_.erase(filesCreated_.begin(), filesCreated_.end());
}
const int Wrapper::save(string myDir) {
  filesCreated_.clear();

  //Save Script file
  if(Log::level()>0) cout << "Wrapper::save() Saving script file " << execName_ <<endl;
  execNameFullHandle_=myDir+"/"+execName_;
  File* myScriptFile = new File(execNameFullHandle_); //deleted in ~Wrapper
  if(script()) return EXIT_FAILURE;
  if(myScriptFile->save(script_)) return EXIT_FAILURE;
  filesCreated_.insert(myScriptFile);
  
  //Make script executable
  if(myScriptFile->makeExec()) return EXIT_FAILURE;

  //Save Steer file
  string steerFileName=execName_ + ".steer";
  if(Log::level()>0) cout << "Wrapper::save() Saving steer file " << steerFileName <<endl;
  File* mySteerFile = new File((myDir+"/"+steerFileName)); //deleted in ~Wrapper
  if(steer()) return EXIT_FAILURE; //steer() overridden by sub-classes
  if(mySteerFile->save(steer_)) return EXIT_FAILURE;
  filesCreated_.insert(mySteerFile);
  return EXIT_SUCCESS;
}

int Wrapper::script() {
  script_="";
  ostringstream os;
  os<<"name="<<"'"<<type_<<"'";
  vector<string> myResults;
  if(LocalDb::instance()->tableRead("Analy_Wrapper", "script", os.str(), myResults)) return EXIT_FAILURE;
  if(myResults.size()!=1) {
    cerr<<"Wrapper::script() Error retrieving from db wrapper script named "<<type_<<endl;
    return EXIT_FAILURE;
  }
  script_=myResults[0];
  return EXIT_SUCCESS;
}
