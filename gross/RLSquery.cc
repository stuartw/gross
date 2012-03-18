#include "RLSquery.hh"

using namespace pool;
using namespace std;

RLSquery::RLSquery() {
  catalog_url = "";
  catalog_type = "";
}

RLSquery::~RLSquery() {
  if(Log::level()>2) cout << "RLSquery::~RLSquery() disconecting and exiting catalog" << endl;
  catalog->disconnect();
  delete catalog;
}

  int RLSquery::initLocalCatalog(File& xmlFile, bool create) {
    string contact="file:" + xmlFile.fullHandle();
    return init(contact, true);
  //catalog_type = "xmlcatalog";
  //catalog_url = "file:" + xmlFile.fullHandle();
  //bool exists = FCSystemTools::FileExists(xmlFile.fullHandle().c_str());
  //if(Log::level()>0) std::cout <<"RLSquery::initLocalCatalog connecting to catalogue "<<catalog_url<<std::endl;
  //try{
    //catalog = FCSystemTools::createCatalog(catalog_type);
    //catalog->connect(catalog_url);
    //catalog->start();
    //if(Log::level()>2) std::cout << "RLSquery::initRLSCatalog connected to catalogue" << std::endl;
  //}catch (const seal::Exception& er){
    //er.printOut(std::cerr);
    //std::cerr << std::endl;
    //if (er.code().isError()){
      //exit(er.code().code());
    //}
  //}
  /*if (create !=  exists)*/ return 0; //no error
  //else return 1; //error
  }

  /*int RLSquery::initRLSCatalog(const string& host) {
  catalog_type = "edgcatalog";
  catalog_url = host;
  if(Log::level()>0) std::cout <<"RLSquery::initRLSCatalog connecting to catalogue "<<catalog_url<<std::endl;
  try{
    catalog = FCSystemTools::createCatalog(catalog_type);
    catalog->connect(catalog_url);
    catalog->start();
    if(Log::level()>2) std::cout << "RLSquery::initRLSCatalog connected to catalogue" << std::endl;
  }catch (const seal::Exception er){
    er.printOut(std::cerr);
    std::cerr << std::endl;
    if (er.code().isError()){
      //exit(er.code().code());
      std::cerr << " RLSquery::initRLSCatalog - connection failed"<<std::endl;
      return 1;
    }
  }
  return 0;
  }*/

int RLSquery::init(const string& contact, bool create) {
  try{
    if(Log::level()>0) std::cout <<"RLSquery::init connecting to catalogue "<<catalog_url<<std::endl; 
    //catalog->setWriteCatalog(contact);
    //catalog->setAction(lookup);
    catalog = new IFileCatalog;
    catalog->addReadCatalog(contact); 
    catalog->setAction(lookup);
    catalog->connect();
    catalog->start();
  }catch (const seal::Exception er){
    er.printOut(std::cerr);
    std::cerr << std::endl;
    if (er.code().isError()){
      std::cerr << " RLSquery::init - connection failed"<<std::endl;
      return 1;
    }
  }
  return 0;
}

vector<string> RLSquery::listLFNs(const string& query) {
  try{
    LFNContainer lfns(catalog, 100);
    vector<string> vecLFN;
    vector<string> queries = or_parser(query);
    for (int i=0; i<queries.size(); i++) {
      lookup.lookupLFNByQuery(queries[i], lfns); 
      while(lfns.hasNext()) {
        stringstream stream;
        LFNEntry lfn=lfns.Next();
        stream << lfn.lfname();
        string result = stream.str();
        vecLFN.push_back(result);
      }
    }
    return vecLFN;
  }catch(const seal::Exception& er){
    er.printOut(std::cerr);
    std::cerr << std::endl;
    if (er.code().isError()){
      exit(er.code().code());
    }
  }
}

vector<string> RLSquery::listPFNs(const string& query) {
  try{
    //catalog->setAction(lookup);
    PFNContainer pfns(catalog, 100);
    vector<string> vecPFN;
    vector<string> queries = or_parser(query);
    for (int i=0; i<queries.size(); i++) {
      lookup.lookupPFNByQuery(queries[i], pfns);
        while(pfns.hasNext()) {
        stringstream stream;
        PFNEntry pfn=pfns.Next();
        stream << pfn.pfname();
        string result = stream.str();
        vecPFN.push_back(result);
      }
    }
    return vecPFN;
  }catch(const seal::Exception& er){
    er.printOut(std::cerr);
    std::cerr << std::endl;
    if (er.code().isError()){
      exit(er.code().code());
    }
  }
}

vector<int> RLSquery::listRuns(const string& query) {
  try{
    list<int> listRun;
    vector<string> queries = or_parser(query);
    for (int i=0; i<queries.size(); i++) {
      MetaDataContainer metas(catalog, 100);
      lookup.lookupMetaDataByQuery(queries[i], metas);
      while(metas.hasNext()) {
        MetaDataEntry attrs=metas.Next();
        AttributeListSpecification spec=attrs.spec();
        AttributeListSpecification::iterator i;
        for(i=spec.begin(); i!=spec.end(); ++i){
          string colname=(*i).name();
	  string colvalue=attrs.getAttributeValueAsString(colname);
	  if (colname == "runid" && colvalue != "") listRun.push_back(atoi(colvalue.c_str()));
        }        
      }
    }
    listRun.sort(); listRun.unique();
    vector<int> vecRun(listRun.begin(), listRun.end());   
    return vecRun;
  }catch(const seal::Exception& er){
    er.printOut(std::cerr);
    std::cerr << std::endl;
    if (er.code().isError()){
      exit(er.code().code());
    }
  }
		      
}

int RLSquery::publishLocalCatalog(File& file, const string& query, bool create) {
  if(Log::level()>2) std::cout << 
		       "RLSquery::publishLocalCatalog writing to " 
			       << file.fullHandle() << " with query " 
			       << query << std::endl;
  try{
    string fullpath = file.fullHandle();
      newcatalog = new IFileCatalog;
      newcatalog->setWriteCatalog("file:" + fullpath);
      FCAdmin admin;
      newcatalog->setAction(admin);
      newcatalog->connect();
      newcatalog->start();
    MetaDataEntry spec;
    lookup.getMetaDataSpec(spec);
    admin.createMetaDataSpec(spec);    
    vector<string> queries = or_parser(query);
    for (int i=0; i<queries.size(); i++) {
      newcatalog->importCatalog(catalog, queries[i]);
    }
    //newcatalog->importCatalog(catalog, query);
    if(Log::level()>2) cout << "RLSquery:: publishLocalCatalog() disconnect from local cat" << endl;
    newcatalog->commit();
    newcatalog->disconnect();
    delete newcatalog;
    return 0;
    }catch(const seal::Exception& er){
    er.printOut(std::cerr);
    std::cerr << std::endl;
    if (er.code().isError()){
      //exit(er.code().code());
      return 1;
    }
    }
}
  
  
vector<string> RLSquery::or_parser(string query) {
  string spacer = " OR ";
  vector<string> ors;
  for (int i=0, j=query.size(); i < query.size();) {
    if ((j = query.find(spacer, i)) != string::npos) {
    } else {
      j = query.size();
    }
    int diff = j - i;
    //if(Log::level()>2) cout << "RLSquery:: publishLocalCatalog() query is: " << query.substr(i, diff) << endl;
    ors.push_back(query.substr(i, diff));
    i = j + 4;
  }
  return ors;
}
						
			
