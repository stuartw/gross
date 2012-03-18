#include "JDL.hh"

#include "File.hh"

JDL::JDL(const string myFileName) :  name_(myFileName), fullHandle_(myFileName) {};
JDL::~JDL() {};

int JDL::save(string myDir) {
  fullHandle_=myDir + "/" + name_;
  if(Log::level()>0) cout << "JDL::save() Saving JDL file " << fullHandle_ <<endl;
  File myFile(fullHandle_);

  if(myFile.save(script())) return EXIT_FAILURE; //Call overridden sub-class function which does the work

  return EXIT_SUCCESS;
}



