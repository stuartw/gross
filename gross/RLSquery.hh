#ifndef RLSQUERY_H
#define RLSQUERY_H

#include <string>
#include <iostream>
#include <sstream>
#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/FCSystemTools.h"
#include "FileCatalog/FCEntry.h"
#include "FileCatalog/URIParser.h"
#include "FileCatalog/IFCAction.h"
#include "FileCatalog/IFCContainer.h"
#include "File.hh"
#include "Log.hh"
#include <list>


class RLSquery{
public:
  //Constructor
  RLSquery();
  
  //destrictor
  ~RLSquery();
  
  //connect to local file, say if it needs to be created or not.
  //on error returns 1
  int initLocalCatalog(File& xmlFile, bool create=false);
  
  //connect to remote EDG RLS - always returns 0 (success)
  //int initRLSCatalog(const std::string& host);
  
  //init local or remote rls or file
  int init(const std::string& contact, bool create=false);

  //List LFN's
  std::vector<std::string> listLFNs(const std::string& query);
  
  //List PFN's 
  std::vector<std::string> listPFNs(const std::string& query);

  //List runs
  std::vector<int> listRuns(const std::string& query);
  
  //copy catalog fragment and publish to local catalog created for this purpose
  int publishLocalCatalog(File& file, const std::string& query, bool create=false);


private:
  std::vector<std::string> or_parser(std::string query); 

  std::string catalog_type;
  std::string catalog_url;
  pool::IFileCatalog* catalog;
  pool::IFileCatalog* newcatalog;
  pool::FClookup lookup;


};



#endif    
