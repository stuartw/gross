// /////////////////////////////////////////////////////////////////////
// Program: Utilities
// Version: 1.0
// File:    OperatingSystem.cpp
// Authors: Claudio Grandi (INFN BO)
// Date:    15/03/2004
// Note:
// /////////////////////////////////////////////////////////////////////

#include "OperatingSystem.h"

#include "Process.h"

#include <iostream>

#include <pwd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>

#include "pstream.h"

using namespace std;
using namespace OSUtils;

string
OSUtils::getUserName() {

  string ret_val = "unknown";
  uid_t uid = getuid();
  ret_val = string("user_")+convert2string(int(uid));
  if (uid >= 0) {
    struct passwd *pwd;
    pwd = getpwuid(uid);
    if ( pwd == NULL ) {
      cerr << "getpwuid: System Call Error: " << strerror(errno) << endl;
    } else {
      ret_val = pwd->pw_name;
    }
  }
  return ret_val;
}


string
OSUtils::getHostName() {

  char buffer[128];
  if ( gethostname(buffer,128) ) {
    cerr << "gethostname: System Call Error: " << strerror(errno) << endl;
    return ""; 
  } 

  return buffer;
}

int
OSUtils::getPid() {

  return getpid();
}

string 
OSUtils::getCurrentDir() {

  char buffer[128];
  if ( getcwd(buffer,128) == NULL ) {
    cerr << "getcwd: System Call Error: " << strerror(errno) << endl;
    return "";
  }

  return buffer;
}

string
OSUtils::getEnv(string env) {

  char* gte = getenv(env.c_str() );
  if ( gte )
    return gte;
  else 
    return "";
}

int
OSUtils::setEnv(string name, string value) {
  return setenv(name.c_str(),value.c_str(),1);
}

string
OSUtils::expandEnv(const string& s) {
  string expanded;
  string var;
  unsigned int len = s.length();
  
  for(unsigned int k = 0; k < len; k++) {
    if(k < len-1 && s[k] == '$') { 
      var = "";
      ++k;
      if(s[k] == '{')
        ++k;
      while(k < len && (isalnum(s[k]) || s[k] == '_'))
        var += s[k++]; 
      if(k < len && s[k] != '}')
        --k; 
      expanded += getEnv(var);
    }
    else
      expanded += s[k];
  }

  return expanded;
}

int 
OSUtils::changeDir(string dir) {

  if ( chdir(dir.c_str()) ) {
    cerr << "chdir: System Call Error: " << strerror(errno) << endl;
    return errno;
  }

  return 0;
}

time_t 
OSUtils::getTime(void) {

  return time(NULL);
}

string
OSUtils::getStrTime(void) {

  string d;
  time_t tt;

  tt = getTime();
// the return value points to a statically  allocated
// string  which  might be overwritten by subsequent calls to
// any of the date and time  functions.
  d = ctime(&tt);
  return d.substr(0,d.size()-1);
}

string 
OSUtils::time2StrTime(time_t* time) {

  string d = ctime(time);

  return d.substr(0,d.size()-1);
}

int
OSUtils::fileSize(string fname, unsigned int *size) {

  if ( fileExist(fname) ) {
    struct stat for_len; 
    if (stat (fname.c_str(),&for_len) == -1) 
      return 0; 
    *size = for_len.st_size;  
    if (!(*size)) 
      return 0;
    return 1;
  } else
    return 0;
}
int 
OSUtils::dirExist(string dname) {
  int ret = 1;
  struct stat buf;
  stat(dname.c_str(),&buf);
  if ( !S_ISDIR(buf.st_mode) )
    ret = 0;
  return ret;
}

int
OSUtils::fileExist(string fname) {
  int ret = 0;
  struct stat buf;
  if ( stat(fname.c_str(),&buf) == 0 )
    ret = 1;
  return ret;
}

bool
OSUtils::isMine(string name) {
   int ret = false;
  struct stat buf;
  stat(name.c_str(),&buf);
  if ( buf.st_uid == getuid() )
    ret = true;
  return ret;
}

int
OSUtils::fileCopy(string src, string dst) {

  return system(string("cp "+src+" "+dst).c_str());

}

int
OSUtils::makeDir(string name) {

  return system(string("mkdir -p "+name+" ").c_str());

}

int
OSUtils::fileRemove(string fname) {

  return system(string("rm -f "+fname).c_str());
}

int
OSUtils::dirRemove(string dname) {
  
  return system(string("rm -rf "+dname).c_str());
}

int
OSUtils::fileChmod(string mode, string dst) {

  return system(string("chmod "+mode+" "+dst).c_str());

}

string 
OSUtils::basename(string fnam) {
  int siz=fnam.size();
  int pos=fnam.find_last_of('/',siz);
  if(pos>=0&&pos<siz)  fnam=fnam.substr(pos+1,siz-pos);
  return fnam;
}

string 
OSUtils::dirname(string fnam) {
  string dir = "";
  int siz=fnam.size();
  int pos=fnam.find_last_of('/',siz);
  if(pos>=0&&pos<siz)  dir=fnam.substr(0,pos);
  return dir;
}

int 
OSUtils::makeFIFO(string file, int mode) {
  mkfifo (file.c_str(),mode);
    // LOGGING
#ifdef LOGL3
  cout << "Created " << file << " pipe" << endl;
#endif
  return 0;
}

int
OSUtils::shell(string comm) {
  return system(comm.c_str());
}

int
OSUtils::forkProcess(OSUtils::Process* proc) {
  // fork the process
  int pid;
  if ( (pid = fork()) < 0 ) {
    cerr << "Cannot fork. System error " << strerror(errno) << endl;
    return -1;
  }
  if ( pid == 0 ) {
    proc->execute();
    exit(0);
  } else {
    cout << "Forked pid " << pid << endl;
    proc->setPid(pid);
    return 0;
  }
}

int
OSUtils::waitProcess(OSUtils::Process* proc, string option) {
  int ret_val = 0;
  int pid = proc->getPid();
  cout << "Wait for pid " << pid << endl;
  int status = 1;
  string ret_code;
  int opt = 0;
  if      (option=="WNOHANG")
    opt = WNOHANG;
  else if (option=="WUNTRACED")
    opt = WUNTRACED;
  int ret_pid = waitpid(pid, &status, opt);
  if      ( ret_pid == 0 && option == "WNOHANG") {
    ret_val = 1;
    cerr << pid << " did not finish yet..." << endl;
  } else if ( ret_pid != pid ) {
    cerr << "Abnormal end of process " << pid << ". Return pid is: " << ret_pid << endl;
    ret_val = -1;
  }
  // intercept the exit code
  string WT;
  int WC;
  if ( WIFEXITED(status)   != 0 ) {
    WT = "";
    WC = WEXITSTATUS(status);
  } else if ( WIFSIGNALED(status) != 0 ) {
    WT = "SIGNAL ";
    WC = WTERMSIG(status);
  } else if ( WIFSTOPPED(status)  != 0 ) {
    WT = "STOP ";
    WC = WSTOPSIG(status);
  } else {
    WT = "UNKNOWN ";
    WC = -999;
  }
  // std::cerr << WT << WC << std::endl;
  proc->setRetCode(WT+" "+convert2string(WC));
  return ret_val;
}

int
OSUtils::waitProcessMaxTime(OSUtils::Process* proc, int maxtime) {
  int ret_val = 0;
  const int interval = 5; // one retry every 5 seconds
  int maxretry = abs(maxtime)/interval + 1;
  if (proc) {
    int retry = 0;
    while ( ( (ret_val=waitProcess(proc,"WNOHANG")) == 1) && 
            retry++ < maxretry ) {
      sleep(interval);
    }
    if (retry>=maxretry) {
      ret_val = terminateProcess(proc);
    }
  }
  return ret_val;
}

std::string
OSUtils::checkProcess(OSUtils::Process* proc) {
  std::string ret_val = "unknown";
  if (waitProcess(proc,"WNOHANG") == 1 )
    ret_val = "running";
  else
    ret_val = proc->retCode();
  return ret_val;
}

int 
OSUtils::terminateProcess(OSUtils::Process* proc) {
  int pid = proc->getPid();
  cout << "Terminate pid " << pid << endl;
  int ret = kill(pid,15);
  if(ret)
    ret += kill(pid,9);
  proc->setRetCode("killed by user"); // 0 ok, -1 killed with sig=9, -2 error
  return ret;
}

vector<string> 
OSUtils::splitString(const string& s, char c) {
  vector<string> ret;
  unsigned int i = 0;
  unsigned int n = s.size();
  while (i < n ) {
    unsigned int j = s.find_first_of(c,i);
    j = j<n ? j : n;
    string s1 = s.substr(i,j-i);
    if (s1.size() > 0 )
      ret.push_back(s1);
    i = j+1;
  }
  return ret;
}

void
OSUtils::trim(string& str) {
  string dummy;
  for (unsigned int i=0; i<str.size(); i++ ) {
    char ch = str[i];
    if ( ch != '\n' && ch != '\t' && ch != ' ' )
      dummy += ch;
  }
  str.assign(dummy,0,dummy.size());
}

void
OSUtils::sleep(unsigned delay) {
  system((string("sleep ")+convert2string(delay)).c_str());
}

int
OSUtils::tar(string opt, string name, const vector<string>& files) {
  string command = "tar "+opt+"f "+name;
  vector<string>::const_iterator it;
  for (it=files.begin(); it<files.end(); it++)
    command += " "+(*it);
  // DEBUG
  // std::cout << command << std::endl;
  // END DEBUG
  return system(command.c_str());
}

int
OSUtils::append(string file, string str) {
  string command = "echo \""+str+"\" >> "+file;
  return system(command.c_str());
}

string 
OSUtils::which(string file) {
  string ret = "";
  string command = "which " + file;
  redi::ipstream psub(command.c_str());
  psub >> ret;
  psub.close();
  if ( !fileExist(ret) )
    ret = "";
  return ret;
}

int
OSUtils::touch(string file) {
  return system(string("touch "+file).c_str());
}

char* 
OSUtils::string2char(string s) {
  char* return_val = 0;
  if ( s != "NULL" ) {
    return_val = new char[s.size()+1]; 
    strcpy(return_val,s.c_str());
  }
  return return_val;
}

int 
OSUtils::string2int(string s) {
  return atoi(string2char(s));
}

// void
// OSUtils::getCommandOutput(std::string command, std::iostream& s) {
//   redi::ipstream ips(command.c_str());
//   std::string line;
//   while(std::getline(ips,line))
//     s << line << std::endl;
// }
