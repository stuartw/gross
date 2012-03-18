#ifndef DELETEPTR_H
#define DELETEPTR_H

/*!
  \brief (Predicate) function to be used when you want to delete containers of pointers to objects

  The predicate part of this function can be used when deleting a vector - you can do the whole
  delete operation in one line. However with associative containers such as Sets (which are 
  constant objects), you cannot use the remove algorithm and are forced instead to use for_each
  which doesn't need the predicate part of this function, but can be used anyway.

*/

template <typename T>
bool deletePtr(T* ptrT) {
  if(ptrT) delete ptrT;
  return true; //Disguise as a predicate function for ease of use when used with remove_if algorithm.
};
  
#endif
