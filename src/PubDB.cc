#include "curl/curl.h"
#include "PubDB.hh"
#include "Log.hh"
#include <list>

using namespace std;

std::string PubDB::tmp_curl_string;

size_t PubDB::curl_fun(void *ptr, size_t size, size_t nmemb, void *stream){
    char *ch = static_cast<char*>(ptr);
    for ( size_t i=0; i<size*nmemb; i++ ){ PubDB::tmp_curl_string.push_back(ch[i]); }
    return size*nmemb;
  }

string PubDB::findChain(const string &findMotherPHP, const string &cid){
  PubDB::tmp_curl_string.clear(); //must put it here, curl sometime retrieves line by line 
  CURL *curl;
  curl = curl_easy_init();
  if (curl) {
    const string url=findMotherPHP+"?cid="+cid;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, PubDB::curl_fun);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    if(Log::level()>4) 
      cout << " ... Downloading " << url  <<endl;
    curl_easy_perform(curl);
    double connectTime(0);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &connectTime);
    if(connectTime == 0 ) {
    if(Log::level()>2) 
      cout << "???? this web site has no response within 10 seconds, so stop connecting"<<endl;
    }
    curl_easy_cleanup(curl);
  }
  string RefDB_string = PubDB::tmp_curl_string;

  if(RefDB_string.find(cid)==string::npos){
    return "0";
  }
  string::size_type first = RefDB_string.find("Type=");
  if(first==string::npos){
    cerr<<"???? Something wrong with the web "<<findMotherPHP<<endl;
    return "0";
  }
  string::size_type second = RefDB_string.find(",", first);
  if(second==string::npos){
    return "0";
  }
  string chain = RefDB_string.substr(first+5, second-first-5);
  return chain;
}

string PubDB::getMotherOwner(const string &findMotherPHP, const string &thisID){
  PubDB::tmp_curl_string.clear(); //must put it here, curl sometime retrieves line by line 
  CURL *curl;
  curl = curl_easy_init();
  if (curl) {
    const string url=findMotherPHP+"?cid="+thisID;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, PubDB::curl_fun);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    if(Log::level()>4) 
       cout << " ... Downloading " << url  <<endl;
    curl_easy_perform(curl);
    double connectTime(0);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &connectTime);
    if(connectTime == 0 ) {
      if(Log::level()>2) 
	cout << "???? this web site has no response within 10 seconds, so stop connecting"<<endl;
    }
    curl_easy_cleanup(curl);
  }
  string RefDB_string = PubDB::tmp_curl_string;

  if(RefDB_string.find("ID=")==string::npos){
    return "";
  }
  string::size_type first = RefDB_string.find("Owner=");
  if(first==string::npos){
    cerr<<"???? Something wrong with the web "<<findMotherPHP<<endl;
    return "";
  }
  string::size_type second = RefDB_string.find("Owner=", first+6);
  if(second==string::npos){
    return "";
  }
  string::size_type end = RefDB_string.find(",", second);
  string motherOwner = RefDB_string.substr(second+6, end-second-6);
  return motherOwner;
}

string PubDB::get_ID_from_RefDB( const string &aRefDB, const string &aOwner, const string &aDataset){  
  PubDB::tmp_curl_string.clear(); //must put it here, curl sometime retrieves line by line 
  CURL *curl;
  curl = curl_easy_init();
  if (curl) {
    const string url=aRefDB+"?Owner="+aOwner+"&Dataset="+aDataset;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, PubDB::curl_fun);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    if(Log::level()>4) 
      cout << " ... Downloading " << url  <<endl;
    curl_easy_perform(curl);
    double connectTime(0);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &connectTime);
    if(connectTime == 0 ) {
      if(Log::level()>2) 
	cout << "???? this web site has no response within 10 seconds, so stop connecting"<<endl;
    }
    curl_easy_cleanup(curl);
  }
  string RefDB_string = PubDB::tmp_curl_string;
   if(RefDB_string.find(":")==string::npos){
     return "0";
   }
  string::size_type first = RefDB_string.find(":")+1;
  string::size_type second = RefDB_string.find("\n", first);
  string rawID = RefDB_string.substr(first, second-first);
  string::size_type first_rawID = rawID.find_first_not_of(" ");
  string ID = rawID.substr(first_rawID, rawID.length());
  return ID;
}

vector<string> PubDB::get_all_PubDbs_from_centralPubDB(const string &aCentralPubDB, const string &aCollectionID){
  PubDB::tmp_curl_string.clear(); //must put it here, curl sometime retrieves line by line 
  CURL *curl;
  curl = curl_easy_init();
  if (curl) {
    const string url=aCentralPubDB+"?CollID="+aCollectionID;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, PubDB::curl_fun);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    if(Log::level()>4) 
      cout << " ... Downloading " << url  <<endl;
    curl_easy_perform(curl);
    double connectTime(0);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &connectTime);
    if(connectTime == 0 ) {
      if(Log::level()>2) 
	cout << "???? this web site has no response within 10 seconds, so stop connecting"<<endl;
    }
    curl_easy_cleanup(curl);
  }
  vector<string> PubDBs;
  string centralPubDB_string = PubDB::tmp_curl_string;

  string::size_type index = centralPubDB_string.find(aCollectionID);
  while(index != string::npos ) 
    {
      string::size_type begin = centralPubDB_string.rfind("http",index);
      if(begin==string::npos)
	break;
      string::size_type end = centralPubDB_string.rfind("/",index)+1;
      string aLine = centralPubDB_string.substr(begin, end-begin);
      PubDBs.push_back(aLine);
      string::size_type end_of_line = centralPubDB_string.find("</TD>",index);
      index = centralPubDB_string.find(aCollectionID,end_of_line);
    }
  if(PubDBs.size()==0 && aCollectionID.length()!=0){
    if(Log::level()>2) 
      cout << "!!!! No PubDB List Found For CollectionID="<<aCollectionID<<endl;
  }
  return PubDBs;
}

string PubDB::get_info_of_aPubDB(const string &aPubDB, const string &aCollectionID){
  PubDB::tmp_curl_string.clear(); //must put it here, curl sometime retrieves line by line 
  CURL *curl;
  curl = curl_easy_init();
  if (curl) {
    const string url=aPubDB+"?CollID="+aCollectionID;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, PubDB::curl_fun);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    if(Log::level()>4) 
      cout << " ... Downloading " << url  <<endl;
    curl_easy_perform(curl);
    double connectTime(0);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &connectTime);
    if(connectTime == 0 ) {
      if(Log::level()>2) 
	cout << "???? this web site has no response within 10 seconds, so stop connecting"<<endl;
    }
    curl_easy_cleanup(curl);
  }
  string site_string = PubDB::tmp_curl_string;
  return site_string;
}

vector<int> PubDB::getAllRuns(const string &aListRuns, const string &aDataset, const string &aCollID){
  vector<int> allRuns;
  PubDB::tmp_curl_string.clear(); //must put it here, curl sometime retrieves line by line 
  CURL *curl;
  curl = curl_easy_init();
  if (curl) {
    const string url=aListRuns+"?DatasetName="+aDataset+"&CollectionID="+aCollID+"&Nb=999999";
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, PubDB::curl_fun);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    if(Log::level()>4) 
      cout << " ... Downloading " << url  <<endl;
    curl_easy_perform(curl);
    double connectTime(0);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &connectTime);
    if(connectTime == 0 ) {
      if(Log::level()>2) 
	cout << "???? this web site has no response within 10 seconds, so stop connecting"<<endl;
    }
    curl_easy_cleanup(curl);
  }
  const string &web = PubDB::tmp_curl_string;
  string key = "RunNum"+aCollID;
  string::size_type keyIndex = web.find(key);
  if(keyIndex==string::npos)
    return allRuns;
  string::size_type keyBefore = web.rfind("<tr", keyIndex);
  string::size_type keyAfter = web.find("</tr", keyIndex);
  if(keyBefore==string::npos||keyAfter==string::npos)
    return allRuns;
  string keySec = web.substr(keyBefore, keyAfter-keyBefore);
  int keyPosition = 0;
  string keySub = " ";
  string::size_type keySubIndex = 0;
  while(keySub.length() != 0){
    keySub = getSubString(keySec, keySubIndex, "<th", "</th>");
    keySubIndex += keySub.length();
    keyPosition++;
    if(keySub.find(key) != string::npos){
      break;
    }
  }

  string section = " ";
  string::size_type index = keyAfter;
  while(section.length()!=0){
    section = getSubString(web, index, "<tr", "</tr>");
    index += section.length();
    string subSec = " ";
    string::size_type subIndex = 0;
    int lineNumber = 0;
    while(subSec.length()!=0){
      subSec = getSubString(section, subIndex, "<td", "</td>");
      lineNumber += 1;
      if(lineNumber == keyPosition){
  	string runSec = getSubString(subSec, 0, "&nbsp;", "&nbsp;");
	if(runSec.length()>0){
	  string runString = runSec.substr(6, runSec.length()-6-6);
	  char **notUsed=0;
	  unsigned long run = strtoul( runString.c_str(), notUsed, 0 );
	  allRuns.push_back(run);
	}
	break;
        }
      string::size_type next = section.find("</td>", subIndex);
      if(next == string::npos){
	break;
      }else{
	subIndex = next+5;
	}
    }
  }
  list<int> tmp(allRuns.begin(), allRuns.end());
  tmp.sort();
  tmp.unique();
  vector<int> allRunsUnique(tmp.begin(), tmp.end());
  return allRunsUnique;
}

string PubDB::getSubString(const string &aString, string::size_type aIndex, const string &head, const string &tail){  
  string::size_type begin = aString.find(head, aIndex);
  string::size_type end   = aString.find(tail, begin+head.length());
  if(begin != string::npos && end != string::npos){
    return aString.substr(begin, end+tail.length()-begin);
  }else{
    return "";
  }
}

vector<string> PubDB::vectorMergeUnique(const vector<string> &a, const vector<string> &b){
  vector<string> tmp = a;
  tmp.insert(tmp.end(), b.begin(), b.end());
  list<string> ltmp(tmp.begin(), tmp.end());
  ltmp.sort();
  ltmp.unique();
  vector<string> vtemp(ltmp.begin(), ltmp.end());
  return vtemp;
}
