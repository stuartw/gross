#include "TaskFactory.hh"

#include "OrcaGFactory.hh"
#include "OrcaLocFactory.hh"

TaskFactory::~TaskFactory() {};

void TaskFactory::del() {
  if(instance_) delete instance_;
  instance_=0;
}
void TaskFactory::facType(string myFacType) {
  facType_=myFacType;
  if(Log::level()>0) cout <<"TaskFactory::facType Setting Factory type to "<<myFacType<<endl;
}

TaskFactory* TaskFactory::instance_=0;
string TaskFactory::facType_="";

TaskFactory* TaskFactory::instance() {
  if(instance_==0) {
    if(facType_.empty()) {
      cerr <<"TaskFactory::instance() Error: Factory type not set!\n";
      abort();
      return instance_;
    }
    if(facType_=="OrcaG") instance_ = new OrcaGFactory(); //Subclass here when required
    if(facType_=="OrcaLoc") instance_ = new OrcaLocFactory();
    
    if(!instance_) cerr << "TaskFactory::instance() Error: Unknown factory type "<<facType_<<endl;
  }
  return instance_;
}

