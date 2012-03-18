#ifndef LOG_H
#define LOG_H

//This header is included in all GROSS classes.
//Thus can add some common declarations:
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <sstream>
using std::ostringstream;
using std::ostream;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::set;

class Log{
private:
  static int level_;
  Log(){}; //Singleton
  static Log* instance_;
public:
  ~Log();
  static Log* instance();
  static int level() {return level_;};
  static void level(int myLevel) {level_=myLevel;};
};


/*
  Verbosity Levels:
  0 : None
  1 : Detailed output for User
  2 : <Spare>
  3 : Fully detailed output for Developer
*/


#endif
