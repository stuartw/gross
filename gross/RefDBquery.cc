#include "RefDBquery.hh"

using namespace std;

RefDBquery::RefDBquery() {
  remoteCatalog="xmlcatalog_http://cmsdoc.cern.ch/cms/production/www/cgi/data/META/VirginMetaCatalog.xml";
  publishDir = "http://cmsdoc.cern.ch/cms/production/publish/PRS/DST";
  if(Log::level()>2) cout << "RefDBquery:: initialsing Refdb interface, using catalog "<< remoteCatalog <<endl;
}

RefDBquery::~RefDBquery() {
  delete localRefDBFrag_;
}

int RefDBquery::init(File& outputPoolFile, const string& myquery, bool create) {
  
  //set owner and dataset from user query
  if (setDatasetOwner(myquery)) return EXIT_FAILURE;

  //find list of owners
  if (setOtherOwners()!=0) return EXIT_FAILURE;
  
  //prepare temporary catalog
  localRefDBFrag_ = new File(FileSys::oDir(), "temp_RefDBFrag.xml");
  if(Log::level()>2) cout << "RefDBquery::init: local cache of RefDB POOL catalog is "<< localRefDBFrag_->name() <<endl;
  if(localRefDBFrag_->remove()) return EXIT_FAILURE; //Must not exist else RLSQuery will fail

  //connect to remote catalog
  if(_remoteRLS.init(remoteCatalog)) return EXIT_FAILURE;
  
  ostringstream os;
  for(vector<string>::const_iterator i = owners.begin(); i<owners.end(); ++i) {
    //os << "lfname like '%.META."+*i+"%' OR lfname like '%META."+dataset+"."+owner+"%' OR ";
    os << "owner='" << *i <<"' AND DataType='META' OR ";
  }
  const string metaquery = os.str();	 
  //  const string metaquery="lfname like '%.META."+owner+"%' OR lfname like '%META."+dataset+"."+owner+"%";
  
  if(Log::level()>0) cout << "RefDBquery::init: querying RefDB catalog with query "<< metaquery <<endl;
  //publish results of query to local file
  if(_remoteRLS.publishLocalCatalog(*localRefDBFrag_, metaquery, true)) return EXIT_FAILURE;
  if(Log::level()>0) cout << "RefDBquery::init: published RefDB results to  "<< localRefDBFrag_->name() <<endl;
  
  //connect to local RLS catalog
  if(_localRLS.init("file:"+localRefDBFrag_->fullHandle())) return EXIT_FAILURE;
  
  //Check dataset is published
  if(dirListing()) {
    cerr << "RefDBquery::init: Unable to find published dataset" <<endl;
    return EXIT_FAILURE;
  }
  
  //make sure files exist
  metaFiles = _localRLS.listLFNs("DataType='META'");
  
  if (metaFiles.size()==0) {
    cerr << "RefDBquery::init: Error no metadata files found"<<endl;
    return EXIT_FAILURE;
  }
  
  for(vector<string>::const_iterator i = metaFiles.begin(); i<metaFiles.end(); ++i) {
    if (!fileExists(*i)) {
      if(Log::level()>2) cout << "RefDBquery::init: Metafile  "<< *i << " found"  <<endl;
    }
    else {
      cerr << "RefDBquery::init: Error dataset: " << dataset << " & owner: " << owner << " not published ";
      cerr << "unable to find file "<< *i <<endl;
      return EXIT_FAILURE;
    }
  }
  if(Log::level()>0) cout <<"RefDBquery::init: Metadata for dataset is published" <<endl;
  
  //finally publish to local cat given to function
  if(_localRLS.publishLocalCatalog(outputPoolFile, "DataType='META'", create)) return EXIT_FAILURE;


  return EXIT_SUCCESS;
}

int RefDBquery::setDatasetOwner(const string query) {
  int first =query.find("\'",query.find("dataset="))+1;
  int second=query.find("\'",first);
  dataset=query.substr(first,second-first);
  first =query.find("\'",query.find("owner="))+1;
  second=query.find("\'",first);
  owner=query.substr(first,second-first);
  
  if (dataset.empty()&&owner.empty()) return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

int RefDBquery::dirListing() {
  CURL *curl;
  CURLcode res;
  curl = curl_easy_init();
  const string url = publishDir+"/"+owner+"/"+dataset+"/";
  FILE *file;
  long httpCode;
  file = fopen((FileSys::oDir()+"/"+owner+"."+dataset+".METAs.txt").c_str(),"w+");
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FILE, file);
    if(Log::level()>2) cout << "RefDBquery::dirListing: Downloading directory " << url  <<endl;
    res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &httpCode);
    fclose(file);
    curl_easy_cleanup(curl);
    if (res==CURLE_OK&&httpCode==200) return EXIT_SUCCESS;
    else if (httpCode==404) cerr << "RefDBquery::dirListing: dataset not published in RefDB"<<endl;
    else cerr << "RefDBquery::dirListing: RefDB responded with error code "<< httpCode <<endl;
    return EXIT_FAILURE;
  }
  
}
int RefDBquery::fileExists(const string filename) {
  ifstream filestream((FileSys::oDir()+"/"+owner+"."+dataset+".METAs.txt").c_str());
  if (!filestream) return EXIT_FAILURE;
  string s;
  int found=string::npos;
  if(Log::level()>2) cout << "RefDBquery::fileExists: looking for " << filename <<endl;
  while (getline(filestream,s)) {
    int temp = s.find(filename.c_str());
    if (temp!=string::npos) found = temp;
  }
  if (found!=string::npos) return EXIT_SUCCESS;
  else return EXIT_FAILURE;
}

int RefDBquery::setOtherOwners() {
  CURL *curl;
  int res;
  curl = curl_easy_init();
  const string url = "http://cmsdoc.cern.ch/cms/production/www/cgi/data/AnaInfo.php?OwnerName="+owner+"&DatasetName="+dataset;
  FILE *file;
  file = fopen((FileSys::oDir()+"/"+owner+"."+dataset).c_str(),"w+");
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FILE, file);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
   fclose(file);
  }
  if (res!=CURLE_OK) return EXIT_FAILURE;
  ifstream filestream((FileSys::oDir()+"/"+owner+"."+dataset).c_str());
  if (!filestream) return EXIT_FAILURE;
  string s;
  while (getline(filestream, s)) {
    int first = s.find("OwnerName=")+10;
    if (first != string::npos+10)   owners.push_back(s.substr(first,100));
  }
  return EXIT_SUCCESS;
}

