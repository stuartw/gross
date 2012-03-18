#ifndef REFDBQUERY_H
#define REFDBQUERY_H

#include "RLSquery.hh"
#include "curl/curl.h"
#include <string>
#include "FileSys.hh"
#include "Log.hh"
#include "File.hh"
#include <fstream>

class RefDBquery{
public:
  RefDBquery();
  ~RefDBquery();
  int init (File& outputPoolFile, const std::string& myquery, bool create=false); //searches RefDB finds Meta LFN adds to POOL cat given

private:
  
  int dirListing(); 
  int fileExists(const string filename);
  int setDatasetOwner(const std::string query);
  int setOtherOwners();
  
  RLSquery _remoteRLS;
  RLSquery _localRLS;
  std::string remoteCatalog;
  File* localRefDBFrag_;
  vector<std::string> metaFiles;
  std::string publishDir;
  std::string owner;
  std::string dataset;
  vector<std::string> owners;
};

#endif
