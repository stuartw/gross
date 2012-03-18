#include "FileSys.hh"

//defaults
FileSys* FileSys::instance_ = 0;
string FileSys::oDir_ = "./";

int FileSys::oDir(string myoDir,bool create) {
  struct stat buf;

  //Ensure it exists
  stat(myoDir.c_str(),&buf);
  if ( !S_ISDIR(buf.st_mode) ) {
    if (create) {
	if(system(string("mkdir -p "+myoDir).c_str())) {
        cerr<<"Error creating directory " << myoDir << endl;
	return EXIT_FAILURE;
        } else {
	if(Log::level()>0) cout << "Directory " << myoDir << " created" <<endl;
        }
    } else return EXIT_FAILURE;
  }
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

string FileSys::workingDir() {
  char dir[1024];
  if (getcwd(dir, sizeof(dir)) == NULL) {
    if(Log::level()>0) cout <<"Error determinig current directory - set to ./"<<Log::level()<<endl;
    return string("./");
  }
  else return string(dir);
}

