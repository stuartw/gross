#include "Retrieve.hh"

#include "LocalDb.hh"
#include "File.hh"
#include "DeletePtr.hh"
#include <algorithm>

Retrieve::Retrieve(const string myExecName, const string myType) 
  : execName_(myExecName), execNameFullHandle_(myExecName), type_(myType){};

Retrieve::~Retrieve() {
  //delete container of pointers to objects
  for_each(filesCreated_.begin(), filesCreated_.end(), deletePtr<File>);
  filesCreated_.erase(filesCreated_.begin(), filesCreated_.end());
}
const int Retrieve::save(string myDir) {
  filesCreated_.clear();

  //Save Script file
  if(Log::level()>0) cout << "Retrieve::save() Saving script file " << execName_ <<endl;
  execNameFullHandle_=myDir+"/"+execName_;
  File* myScriptFile = new File(execNameFullHandle_); //deleted in ~Wrapper
  if(script()) return EXIT_FAILURE;
  if(myScriptFile->save(script_)) return EXIT_FAILURE;
  filesCreated_.insert(myScriptFile);
  
  //Make script executable
  if(myScriptFile->makeExec()) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

int Retrieve::script() {
  script_="";
  ostringstream os;
  os<<"name="<<"'"<<type_<<"'";
  vector<string> myResults;
  if(LocalDb::instance()->tableRead("Analy_Retrieve", "script", os.str(), myResults)) return EXIT_FAILURE;
  if(myResults.size()!=1) {
    cerr<<"Retrieve::script() Error retrieving from db wrapper script named "<<type_<<endl;
    return EXIT_FAILURE;
  }
  script_=myResults[0];
  return EXIT_SUCCESS;
}
