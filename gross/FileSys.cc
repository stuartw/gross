#include "FileSys.hh"

//defaults
FileSys* FileSys::instance_ = 0;
string FileSys::oDir_ = "./";

int FileSys::oDir(string myoDir) {
  struct stat buf;

  //Ensure it exists
  stat(myoDir.c_str(),&buf);
  if ( !S_ISDIR(buf.st_mode) )
    return EXIT_FAILURE;

  //Clean up trailing /
  oDir_=myoDir;
  if(myoDir.find_last_of("/")==(myoDir.length()-1))
    oDir_=myoDir.substr(0,myoDir.length()-1);

  return EXIT_SUCCESS;
}

FileSys* FileSys::instance() {
  if(!instance_)
    instance_ = new FileSys();
  return instance_;
}
