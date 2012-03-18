#ifndef CONTPRINT_HH
#define CONTPRINT_HH
#include "File.hh"

//Global functions to help output containers of various kinds

ostream& operator<<(ostream& os, const vector<string>& myVec) {
  for(vector<string>::const_iterator it=myVec.begin(); it!=myVec.end();it++)
    os<<(*it)<<" ";
  return os;
}
ostream& operator<<(ostream& os, const vector<File*>& myVec) {
  for(vector<File*>::const_iterator it=myVec.begin(); it!=myVec.end();it++)
    os<<(*it)->fullHandle()<<" ";
  return os;
}
ostream& operator<<(ostream& os, const set<string>& myVec) {
  for(set<string>::const_iterator it=myVec.begin(); it!=myVec.end();it++)
    os<<(*it)<<" "/*endl*/;
  return os;
}
ostream& operator<<(ostream& os, const set<File*>& myVec) {
  for(set<File*>::const_iterator it=myVec.begin(); it!=myVec.end();it++)
    os<<(*it)->fullHandle()<<" ";
  return os;
}
ostream& operator<<(ostream& os, const vector<int>& myVec) {
	  for(vector<int>::const_iterator it=myVec.begin(); it!=myVec.end();it++)
		      os<<(*it)<<" ";
	    return os;
}

#endif
