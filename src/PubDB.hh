#ifndef PUBDB_H
#define PUBDB_H
#include <string>
#include <vector>

using namespace std;

class PubDB{
public:

  static string RefDB;
  static string centralPubDB;
  static string listRuns;
  static string getPubDBInfo;
  static string findMotherPHP;

  static string tmp_curl_string;
  static size_t curl_fun(void *ptr, size_t size, size_t nmemb, void *stream);
  static bool PubDB::download(const string &url);

  static string findChain(const string &cid);
  static string getMotherOwner(const string &thisID);
  static string get_ID_from_RefDB(const string &aOwner, const string &aDataset);
  static std::vector<string> get_all_PubDbs_from_centralPubDB(const string &aCollectionID);
  static string get_info_of_aPubDB(const string &aPubDB, const string &aCollectionID);
  static vector<int> getAllRuns(const string &aDataset, const string &aCollID);
  static string getSubString(const string &aString, string::size_type aIndex, const string &head, const string &tail);
  static string get_value(const string &aString, const string &name);
  static vector<string> PubDB::vectorMergeUnique(const vector<string> &a, const vector<string> &b);

private:
  PubDB(){}
  ~PubDB(){}
};
#endif //PUBDB_H

