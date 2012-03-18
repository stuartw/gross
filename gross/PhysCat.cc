#include "PhysCat.hh"

#include "RLSquery.hh"
#include "FileSys.hh"
#include "TaskFactory.hh"
#include "RefDBquery.hh"

PhysCat::~PhysCat() {
  if(localRLSFrag_) delete localRLSFrag_;
  if(localRLSQuery_) delete localRLSQuery_;
}

int PhysCat::init(const string& myRLSCat, const string& myGenQuery) {  
  //Connect to remote RLS
  RLSquery remoteRLS;
  //  if(remoteRLS.initRLSCatalog(myRLSCat)) return EXIT_FAILURE;
  if(remoteRLS.init(myRLSCat)) return EXIT_FAILURE;  
  localRLSFrag_ = new File(FileSys::oDir(), "temp_RLSFrag.xml");
  if(localRLSFrag_->remove()) return EXIT_FAILURE; //Must not exist else RLSQuery will fail

  query=myGenQuery; 

  /***  START OF FACTORY SPECIFC CODE ***/
  //if Grid job contact RefDB to get metadata
    if (TaskFactory::facType() == "OrcaG") {
      RefDBquery refdb;
 
      //connect to refdb and get POOL catalog
      if (refdb.init(*localRLSFrag_, query, !(localRLSFrag_->exists()))) {
        cerr << "PhysCat::init: Error contacting RefDB"<<endl;
        return EXIT_FAILURE;
      }
  
    }//end if Grid job

    //If Local job
    else if (TaskFactory::facType() == "OrcaLoc") {
      query=query+" OR DataType='META' OR pfname='PersilCatalogInit'";
    }//end if local job
  /*** END OF FACTORY SPECIFIC CODE ***/


  //Prepare local Pool copy, with meta data, from refDB or original cat, create metadata spec if doesn't already exist
  if(remoteRLS.publishLocalCatalog(*localRLSFrag_, query, !(localRLSFrag_)->exists())) return EXIT_FAILURE;

  
  //Connect to local fragment
  localRLSQuery_ = new RLSquery;
  if(localRLSQuery_->initLocalCatalog(*localRLSFrag_)) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

const vector<int> PhysCat::runs(const string& myDataQuery) {
  vector<int> myRuns;
  if(!localRLSFrag_ || !localRLSQuery_) {
    cerr << "PhysCat::runs Error - initialisation not complete"<<endl;
    return myRuns;
  }
  if(Log::level()>2) cout << "PhysCat::runs: creating vector of runs for data subquery "<< myDataQuery <<endl;
  myRuns = localRLSQuery_->listRuns(myDataQuery);
  return myRuns;
}

const vector<string> PhysCat::listLFNs(const string& myDataQuery) {
  vector<string> myLFNs;
  if(!localRLSFrag_ || !localRLSQuery_) {
    cerr << "PhysCat::LFNs Error - initialisation not complete"<<endl;
    return myLFNs;
  }
  if(Log::level()>2) cout << "PhysCat::LFNs: creating vector of LFNs for data subquery "<< myDataQuery <<endl;  
  myLFNs = localRLSQuery_->listLFNs(myDataQuery);
  return myLFNs;
}

const vector<string> PhysCat::listPFNs(const string& myDataQuery) {
  vector<string> myPFNs;
  if(!localRLSFrag_ || !localRLSQuery_) {
    cerr << "PhysCat::listPFNs Error - initialisation not complete"<<endl;
    return myPFNs;
  }
  if(Log::level()>2) cout << "PhysCat::listPFNs: creating vector of LFNs for data subquery "<< myDataQuery <<endl;
  myPFNs = localRLSQuery_->listPFNs(myDataQuery);
  return myPFNs;
}

File* PhysCat::xMLFrag(const string& myDataQuery, string aFileName) {
  File* pFile;
  if(!localRLSFrag_ || !localRLSQuery_) {
    cerr << "PhysCat::XMLFrag Error - initialisation not complete"<<endl;
    return pFile;
  }
  if(Log::level()>2) cout << "PhysCat::XMLFrag: creating XMLFrag file for data subquery "<< myDataQuery <<endl;
  pFile = new File(aFileName); //Client must delete
  if(pFile->remove()) return 0; //Must not exist else RLSQuery will fail 
  if(localRLSQuery_->publishLocalCatalog(*pFile, myDataQuery, true)) {
    cerr<<"PhysCat::XMLFrag() Error creating XMLFrag. Potentially uncaught!!"<<endl;
    return 0; //Empty pointer
  }
    
  return pFile;
}

int PhysCat::setDatasetOwner(const string query) {
  int first =query.find("\'",query.find("dataset="))+1;
  int second=query.find("\'",first);
  dataset=query.substr(first,second-first);
  first =query.find("\'",query.find("owner="))+1;
  second=query.find("\'",first);
  owner=query.substr(first,second-first);

  if (dataset.empty()&&owner.empty()) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
