#ifndef FILESYS_H
#define FILESYS_H
#include "Log.hh"

#include <sys/stat.h>

class FileSys{
private:
  static string oDir_;
  FileSys() {}; //Singleton
  static FileSys* instance_;
public:
  ~FileSys();
  static FileSys* instance();
  static string oDir() {return oDir_;};
  static int oDir(string myoDir);
};


#endif
