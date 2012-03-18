#include "File.hh"

#include <sys/stat.h>
#include <fstream>
#include <cstdlib>
using std::ifstream;
using std::ofstream;


File::File(string my_fullHandle) 
  : fullHandle_(my_fullHandle) {

  if(fullHandle_.find_last_of("/")==string::npos) { //If just local file name given then no / in name
    location_ = "./";
    name_ = fullHandle_;
  } else {
    location_ = fullHandle_.substr(0,fullHandle_.find_last_of("/"));
    name_ = fullHandle_.substr(fullHandle_.find_last_of("/")+1,fullHandle_.size());
  }
};

File::File(string my_location, string my_name) 
  : name_(my_name),
    location_(my_location),
    fullHandle_(my_location + "/" + my_name) {};

void File::name(string my_name){
  name_=my_name;
}
void File::location(string my_location){
  location_=my_location;
}
void File::fullHandle(string my_fullHandle){
  fullHandle_=my_fullHandle;
}
const string File::name() const{
  return name_;
}
const string File::location() const{
  return location_;
}
const string File::fullHandle() const{
  return fullHandle_;
}
int File::exists() const {
  int ret = 0;
  struct stat buf;
  if ( stat((string(fullHandle())).c_str(),&buf) == 0 )
    ret = 1;
  return ret;
}
const string File::contents() const {
  string rval = "";
  if(!exists()) {
    cerr << "File::contents(): Error: Cannot find file"<< endl;
    return rval;
  }
  ifstream in(fullHandle().c_str());
  string line;
  while(getline(in, line))
    rval+= line + "\n";
  return rval;
}
int File::save(string myString) const {
  ofstream out(fullHandle().c_str());
  if(!out) {
    cerr<<"File::save() Error in creating file "<<fullHandle()<<endl;
    return EXIT_FAILURE;
  }
  out<<myString;
  return EXIT_SUCCESS;
}
int File::makeExec() const {
  string myString =string("chmod +x")+string(" ")+(fullHandle());
  if(system(myString.c_str())) {
    cerr<<"File::makeExec() Error changing file to executable mode"<<endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
int File::remove() const {
  if(system(string("rm -f "+fullHandle()).c_str())) {
    cerr<<"File::remove() Error removing file named " << fullHandle()<<endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
int File::move(string myDir) {
  if(system((string("mv ") + fullHandle() + string(" ") + myDir).c_str()))  {
    cerr<<"File::move() Error moving file named " << name() << " to " << myDir <<endl;
    return EXIT_FAILURE;
  }
  location_=myDir;
  return EXIT_SUCCESS;
}
