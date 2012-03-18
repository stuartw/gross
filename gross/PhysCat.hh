#ifndef PHYSCAT_H
#define PHYSCAT_H
#include "Log.hh"

class File;
class RLSquery;

class PhysCat {        //This is fake until real interface to Phys Cat is decided.
public:
  PhysCat(){};
  ~PhysCat();
  int init(const string& anRLSCat, const string& aGenQuery); 
  const vector<int> runs(const string& aDataSubQuery);
  const vector<string> filesForRun(const int& aRun) {}; //Obsolete
  const vector<string> listLFNs(const string& aDataSubQuery);
  const vector<string> listPFNs(const string& aDataSubQuery);
  File* xMLFrag(const string& aDataSubQuery, string aFileName);
private:
  int setDatasetOwner(const std::string query); 
  File* localRLSFrag_;
  RLSquery* localRLSQuery_; 
  std::string owner;
  std::string dataset;
  std::string query;
  //No implementation
  PhysCat(const PhysCat&);
  PhysCat& operator=(const PhysCat&);
};

#endif
