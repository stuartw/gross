#ifndef DBORCATASK_H
#define DBORCATASK_H

#include "Log.hh"
#include "DbTask.hh"
#include "PhysCat.hh"

class DbOrcaTask : public DbTask {
public:
  DbOrcaTask(const int& anId) : DbTask(anId) {};
  int split();
  PhysCat* physCat() const {return physCat_;}; ///<Getter
  void physCat(PhysCat* myPhysCat) {physCat_=myPhysCat;}; ///<Setter 
private:
  PhysCat* physCat_;
};
#endif
