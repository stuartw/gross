#ifndef CONTPRINT_H
#define CONTPRINT_H
#include "Log.hh"

class File;

/*!

\brief Some global functions to help output containers of various kinds

*/

ostream& operator<<(ostream& os, const vector<string>& myVec);
ostream& operator<<(ostream& os, const vector<File*>& myVec);
ostream& operator<<(ostream& os, const set<string>& myVec);
ostream& operator<<(ostream& os, const set<File*>& myVec);


#endif
