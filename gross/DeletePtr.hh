#ifndef DELETEPTR_H
#define DELETEPTR_H

template <typename T>
bool deletePtr(T* ptrT) {
  if(ptrT) delete ptrT;
  return true; //Disguise as a predicate function for ease of use
};
  
#endif
