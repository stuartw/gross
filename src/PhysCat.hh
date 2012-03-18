#ifndef PHYSCAT_H
#define PHYSCAT_H
#include <string>
#include <vector>

#include "PubDB.hh"
#include "Site.hh"

using namespace std;

class PhysCat;

class PhysCat{
public:
  static string RefDB;
  static string centralPubDB;
  static string listRuns;
  static string getPubDBInfo;
  static string findMotherPHP;

  PhysCat(const string &owner, const string &dataset);
  PhysCat(const string &owner, const string &dataset, const string &chain);
  ~PhysCat(){}

  vector<int>   getAllRuns();
  vector<Site*> getSites4MyRuns(const vector<int> &runs);
  vector<Site*> getAllSites();

  string getOwner();
  string getDataset();
  string getCollectionID();
  string getChain();
  string getChainCombined();

  void setChainCombined(string &input);

private:
  string m_owner;
  string m_dataset;
  string m_chain;
  string m_chain_combined;
  string m_collectionID;
  bool   m_sites_ready;
  vector<Site*> m_sites;

  PhysCat *physCat_cross_from_two_chain(PhysCat *original, const string &chain2);
  PhysCat *physCat_of_any_chain(const string &owner, const string &dataset, const string &chain);
  bool createAllSites();
  bool createSites_from_siteSet(vector<Site*> &sites);
  bool set_Sites_from_PubDBs(const string &collID, const vector<string> &pubDBs, vector<Site*> &sites);
  bool set_a_site(const string &url, const string &aSiteString, const string &aFileType, Site &aSite);
  string get_value(const string &aString, const string &name)const;
};
#endif //PHYSCAT_H
