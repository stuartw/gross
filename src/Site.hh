#ifndef SITE_H
#define SITE_H
#include <string>
#include <vector>

using namespace std;

class Site{
public:
  Site(){}
  ~Site(){}
  void set_ContactString_chained(const vector<string> &input);
  void set_CE_chained(const vector<string> &input);

  void set_SiteType(const string &input);
  void set_PubDB_URL(const string &input);
  void set_FileType(const string &input);
  void set_ValidationStatus(const string &input);
  void set_ContactString(const string &input);
  void set_ContactProtocol(const string &input);
  void set_CatalogueType(const string &input);
  void set_SE(const string &input);
  void set_CE(const string &input);
  void set_Nevents(const string &input);
  void set_RunRange(const string &input);
  void set_META_site(const void* input);

  string getSiteType()const;
  string getPubDB_URL()const;
  vector<string> getFileType()const;
  vector<string> getValidationStatus()const;
  vector<string> getContactString()const;
  vector<string> getContactProtocol()const;
  vector<string> getCatalogueType()const;
  vector<string> getSE()const;
  vector<string> getCE()const;
  vector<string> getNevents()const;
  vector<string> getRunRange()const;
  vector<int>    getRuns()const;

private:
  string m_SiteType;
  string m_PubDB_URL;
  vector<string> m_ContactString_chained;
  vector<string> m_CE_chained;
  const void *m_META_site;

  string m_FileType;
  string m_ValidationStatus;
  string m_ContactString;
  string m_ContactProtocol;
  string m_CatalogueType;
  string m_SE;
  string m_CE;
  string m_Nevents;
  string m_RunRange;

  string get_FileType()const;
  string get_ValidationStatus()const;
  string get_ContactString()const;
  string get_ContactProtocol()const;
  string get_CatalogueType()const;
  string get_SE()const;
  string get_CE()const;
  string get_Nevents()const;
  string get_RunRange()const;

  const void *get_META()const;
  vector<int> get_runs()const;
};
#endif //SITE_H
