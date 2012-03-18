#include "Log.hh"

int Log::level_=0; //Default Logging Level
Log* Log::instance_=0;

Log* Log::instance() {
  if(!instance_) 
    instance_ = new Log();
  return instance_;
}

