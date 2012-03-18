#include "PhysCat.hh"
#include <iostream>
#include <sstream>
#include <list>
#include "Log.hh"

using namespace std;

string PhysCat::RefDB = "http://cmsdoc.cern.ch/cms/production/www/PubDB/GetIdCollection.php";
string PhysCat::centralPubDB = "http://cmsdoc.cern.ch/cms/production/www/PubDB/GetPublishedCollectionInfoFromRefDB.php";
string PhysCat::listRuns = "http://cmsdoc.cern.ch/cms/production/www/cgi/SQL/List_RUNtable_forBrowsing.php";
string PhysCat::getPubDBInfo =  "get-pubdb-analysisinfo.php";
string PhysCat::findMotherPHP  =  "http://cmsdoc.cern.ch/cms/production/www/cgi/SQL/CollectionTree.php";

PhysCat::PhysCat(const string &owner, const string &dataset){ 
  m_owner = owner; 
  m_dataset = dataset; 
  string collectionID =  PubDB::get_ID_from_RefDB(RefDB, m_owner, m_dataset);
  m_collectionID = collectionID;
  m_chain = PubDB::findChain(findMotherPHP,collectionID);
  m_chain_combined = m_chain;
  m_sites_ready = false; 
//   if(Log::level()>2){
//     cout<<">>>> Dataset="<<m_dataset<<endl;
//     cout<<">>>> Owner="<<m_owner<<endl;
//     cout<<"<<<< CollectionID="<<m_collectionID<<endl;
//     cout<<"<<<< Type="<<m_chain<<endl;
//   }
}

PhysCat::PhysCat(const string &owner, const string &dataset, const string &chain){ 
  string seed=chain;
  if(chain==""||chain==" "||chain=="   "||chain=="    ") seed="";
  if(chain.find("Hit") !=string::npos)  seed="Hit";
  if(chain.find("Digi")!=string::npos)  seed="Digi";
  if(chain.find("DST") !=string::npos)  seed="DST";
  PhysCat *index = physCat_of_any_chain(owner, dataset, seed);
  if(index!=0){
    string originalType = index->getChain();
    PhysCat *temp;
    if(originalType!="DST"&&chain.find("DST")!=string::npos){
      temp = physCat_cross_from_two_chain(index, "DST");
      delete index;
      index=temp;
    }
    if(index!=0&&originalType!="Digi"&&chain.find("Digi")!=string::npos){ 
      temp = physCat_cross_from_two_chain(index, "Digi");
      delete index;
      index=temp;
    }
    if(index!=0&&originalType!="Hit"&&chain.find("Hit")!=string::npos){
      temp = physCat_cross_from_two_chain(index, "Hit");
      delete index;
      index=temp;
    }
    if(index!=0){
      m_owner = index->getOwner(); 
      m_dataset = index->getDataset(); 
      m_collectionID = index->getCollectionID();
      m_chain = index->getChain();
      m_chain_combined = index->getChainCombined();
      m_sites = index->getAllSites();
      m_sites_ready = true;
      delete index;
    }else{
      cout<<"!!!! set dataset=\"\" owner=\"\", procedures will run normally but no data will be returned"<<endl;
    }
  }else{
    cout<<"!!!! set dataset=\"\" owner=\"\", procedures will run normally but no data will be returned"<<endl;
  }
}

PhysCat *PhysCat::physCat_of_any_chain(const string &owner, const string &dataset, const string &chain){  
  PhysCat *phy;
  if(chain==""){
    phy = new PhysCat(owner,dataset);
  }else{
    PhysCat physCat(owner, dataset);
    string theChain = physCat.getChain();
    PhysCat *mother = &physCat;
    while(theChain!=chain){
      string mother_owner = PubDB::getMotherOwner(findMotherPHP, mother->getCollectionID());
      if(mother_owner == ""){
	if(Log::level()>4)
	  cout<<"!!!! Not found  mother Owner for Dataset = "<<dataset<<" Owner = "<<mother->getOwner()<<endl;
	mother=0;
	break;
      }else{
	PhysCat *temp = new PhysCat(mother_owner, dataset);
	mother=temp;
      }
      if(mother==0) break;
      theChain = mother->getChain();
    }
    if(mother!=0){
      string newOwner = mother->getOwner(); 
      phy = new PhysCat(newOwner, dataset);
    }else{
      phy = 0;
      cerr<<"!!!! Not found the data with Type=\""<<chain<<"\" related with data: Dataset="<<dataset<<" Owner="<<owner<<endl;
    }
  }
  return phy;
}

PhysCat *PhysCat::physCat_cross_from_two_chain(PhysCat *original, const string &chain){
  if(Log::level()>2)
    cout<<"**** Trying to find \""<<chain<<"\" data of Dataset="<<original->getDataset()<<endl;
  string owner = original->getOwner();
  string dataset = original->getDataset();
  PhysCat *newPhysCat = physCat_of_any_chain(owner, dataset, chain);
  if(newPhysCat==0){
    return newPhysCat;
  }
  string newChainCombined = original->getChainCombined()+"/"+chain;
  newPhysCat->setChainCombined(newChainCombined);
  vector<Site*> sites1=original->getAllSites();
  newPhysCat->createSites_from_siteSet(sites1);
  vector<Site*> sites2=newPhysCat->getAllSites();
  if(sites2.size()==0){
    string chainCombined = newPhysCat->getChainCombined();
    cerr<<"???? Not found data type of \""<<chainCombined<<"\" with dataset="<<dataset<<endl;
    newPhysCat=0;
    return newPhysCat;
  }
  vector<Site*> newSites;
  for(vector<Site*>::iterator j=sites2.begin(); j!=sites2.end(); j++){
    for(vector<Site*>::iterator i=sites1.begin(); i!=sites1.end(); i++){
      if((**i).getPubDB_URL()==(**j).getPubDB_URL()&&(**i).getFileType()==(**j).getFileType()){
	vector<string> mergedContactString = PubDB::vectorMergeUnique((**i).getContactString(), (**j).getContactString());
	(**j).set_ContactString_chained(mergedContactString);
	vector<string> mergedCE = PubDB::vectorMergeUnique((**i).getCE(), (**j).getCE());
 	(**j).set_CE_chained(mergedCE);
      }
    }
  }
  return newPhysCat;
}

vector<int> PhysCat::getAllRuns(){
  if(!m_sites_ready){ createAllSites(); }
  vector<int> allRuns = PubDB::getAllRuns(listRuns, m_dataset, getCollectionID());
  return allRuns;
}

vector<Site*> PhysCat::getSites4MyRuns(const vector<int> &runs){
  vector<Site*> mySites;
  if(!m_sites_ready){ createAllSites(); }
  for(vector<Site*>::iterator i=m_sites.begin(); i!=m_sites.end(); i++){
    bool hasIt = false;
    vector<int> siteRuns = (**i).getRuns();
    for(vector<int>::const_iterator j=runs.begin(); j!=runs.end(); j++){
      for(vector<int>::iterator k=siteRuns.begin(); k!=siteRuns.end(); k++){
	if(hasIt) break;
	if(*j==*k){
	  hasIt = true;
	  break;
	}
      } 
    }
    if(hasIt) mySites.push_back(*i);
  }
  if(mySites.size()==0){
    if(Log::level()>2) 
      cout<<"!!!! NO Sites found for runs "<<runs[0]<<" ..."<<endl; 
  }
  return mySites;
}

vector<Site*> PhysCat::getAllSites(){
  if(!m_sites_ready){ createAllSites(); }
  return m_sites;
}

bool PhysCat::createSites_from_siteSet(vector<Site*> &sites){
  m_sites_ready=true;
  if(Log::level()>2) 
    cout << ">>>> dataset="<<m_dataset<<" owner=" <<m_owner <<endl;
  string collID = getCollectionID();
  vector<string> PubDBs;
  for(vector<Site*>::iterator i = sites.begin(); i!=sites.end(); i++){
    string url = (**i).getPubDB_URL();
    PubDBs.push_back(url);
  }
  if(PubDBs.size()==0){
    cerr<<"???? The Collectin ID for Dataset = "<<m_dataset<<" Owner = "<<m_owner<<" is found as "<<collID<<endl;
    cerr<<"???? But No PubDB found for this Collection ID "<<collID<<endl; 
    return false;
  }else{
    if(Log::level()>2) 
      cout <<">>>> "<<PubDBs.size()<<" PubDBs taken as input from the already seledted PubDBs"<<endl;
    set_Sites_from_PubDBs(collID, PubDBs, m_sites);
    return true;
  }
}

bool PhysCat::createAllSites(){
  m_sites_ready=true;
  if(m_owner.length()==0||m_dataset.length()==0)
    return false;
  if(Log::level()>2) 
    cout << ">>>> dataset="<<m_dataset<<" owner=" <<m_owner <<endl;
  string collID = getCollectionID();
  if(Log::level()>2) 
    cout << "<<<< Find CollectionID=" << collID<<"  Type="<<getChain() <<endl;
  vector<string> PubDBs = PubDB::get_all_PubDbs_from_centralPubDB(centralPubDB, collID);
  if(PubDBs.size()==0){
    cerr<<"!!!! No PubDB found for CollectionID"<<collID<<endl; 
    return false;
  }else{
    if(Log::level()>2) 
      cout <<"<<<< "<<PubDBs.size()<<" PubDBs of CollectionID="<<collID<<" are found from "<<centralPubDB<<endl;
    set_Sites_from_PubDBs(collID, PubDBs, m_sites);
    return true;
  }
}

bool PhysCat::set_Sites_from_PubDBs(const string &collID, const vector<string> &pubDBs, vector<Site*> &sites){
    for ( vector<string>::const_iterator i=pubDBs.begin(); i!=pubDBs.end(); i++)
      {
	string PubDB=*i+getPubDBInfo;
	string url=*i+getPubDBInfo+"?CollID="+collID;
	string site_string = PubDB::get_info_of_aPubDB(PubDB, collID);
	if ( site_string.find("FileType=Complete") != string::npos )
	  {
	    Site *CompleteSite = new Site;
	    set_a_site(*i, site_string, "FileType=Complete", *CompleteSite);
	    sites.push_back(CompleteSite);
	    if(Log::level()>2) 
	      cout<<"  :-)  "<<url<<" is selected with FileType=Complete"<<endl;
	  } 
	else if ( site_string.find("FileType=AttachedMETA") != string::npos )
	  {
	    Site *METASite = new Site;
	    bool status = set_a_site(*i, site_string, "FileType=AttachedMETA", *METASite);
	    Site *EventsSite = new Site;
	    status = set_a_site(*i, site_string, "FileType=Events", *EventsSite);
	    EventsSite->set_META_site(METASite);   
	    sites.push_back(EventsSite);
	    if(Log::level()>2) 
	      cout<<"  :-)  "<<url<<" is selected with FileType=AttachedMETA & FileType=Events"<<endl;
	  }else{
	    if(Log::level()>2) 
	      cout<<"  :-(  "<<url<<" is NOT selected, it has neither Complete nor META/Events data"<<endl;
	  }
      }
    if(Log::level()>2) 
      cout<<"<<<< "<< sites.size()<<" pubDBs are selected here"<<endl;
    return true;
}

bool PhysCat::set_a_site(const string &url, const string &aSiteString, const string &aFileType, Site &aSite){
  aSite.set_SiteType(m_chain_combined);
  aSite.set_PubDB_URL(url);
  string::size_type begin_block = aSiteString.find(aFileType);
  string::size_type end_block   = aSiteString.find("FileType", begin_block+8);
  if ( end_block == string::npos ) end_block = aSiteString.length();
  string block_string = aSiteString.substr(begin_block, end_block);
  aSite.set_FileType( get_value(block_string,"FileType") );
  aSite.set_ValidationStatus( get_value(block_string,"ValidationStatus") );
  aSite.set_ContactString( get_value(block_string,"ContactString") );
  aSite.set_ContactProtocol( get_value(block_string,"ContactProtocol") );
  aSite.set_CatalogueType( get_value(block_string,"CatalogueType") );
  aSite.set_SE( get_value(block_string,"SE") );
  aSite.set_CE( get_value(block_string,"CE") );
  aSite.set_Nevents( get_value(block_string,"Nevents") );
  aSite.set_RunRange( get_value(block_string,"RunRange") );
  return 1;
}

string PhysCat::get_value(const string &aString, const string &name)const{
  string::size_type begin = aString.find(name)+name.length()+1;
  string::size_type end = aString.find("\n",begin);
  return aString.substr(begin, end-begin);
}


string PhysCat::getOwner(){return m_owner;}
string PhysCat::getDataset(){return m_dataset;}
string PhysCat::getCollectionID(){return m_collectionID;}
string PhysCat::getChain(){return m_chain;}
string PhysCat::getChainCombined(){return m_chain_combined;}

void PhysCat::setChainCombined(string &input){m_chain_combined = input;}
