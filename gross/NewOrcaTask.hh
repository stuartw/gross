#ifndef NEWORCATASK_H
#define NEWORCATASK_H
#include "Log.hh"

#include "NewTask.hh"
class File;
class PhysCat;

class NewOrcaTask : public NewTask {
public:
  NewOrcaTask(const File* userCladFile) : NewTask(userCladFile) {};
  ~NewOrcaTask();
  int split(); ///<Override this method

  PhysCat* physCat() const {return physCat_;}; ///<Getter

  void physCat(PhysCat* myPhysCat) {physCat_=myPhysCat;}; ///<Getter
  
private:
  PhysCat* physCat_; ///<Required by split method
};

#endif
