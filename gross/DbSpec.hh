#ifndef DBSPEC_H
#define DBSPEC_H
#include "Log.hh"

#include "BossClassAd.h"

//! Database connection parameter specification class
/*! This is the specification object for a generic database connection.
  Currently implemented for MySQL, but other connection parameters relevant for 
  other databases could be added transparently in the future.
  A user will specify the connection details for a database in JDL format 
  (passed as a string). This class will then extract and store the relevant 
  connection parameters.
  
  \todo Overload << operator for nice print out of all connection parameters.
  \todo Obsolete this class by implementing in terms of the more general TaskSpec instead.
*/
class DbSpec {
public:
  /*!
    \brief Ctor sets default parameters, creates classad object from JDL and then calls init() function.
    \param aDbSpec is a string containing the database connection parameters passed in JDL format
    \sa init()
   */
  DbSpec(string aDbSpec);
  ~DbSpec(); ///< Dtor deletes all private members.
  ///\brief Getter function
  const string hostname() const {return hostname_;};
  ///\brief Getter function
  const string username() const {return username_;};
  ///\brief Getter function
  const string password() const {return password_;};
  ///\brief Getter function
  const string database() const {return database_;};
  ///\brief Getter function
  const unsigned int port() const {return port_;}; 
  ///\brief Getter function
  const string unixSocket() const {return unixSocket_;};
  ///\brief Getter function
  const long clientFlag() const {return clientFlag_;};
private:
  /*!
    \brief Initialises all connection parameters.

    Note that the BossClassad lookup is read once only (then the successfully read 
    parameter is removed from the clad). Thus once read, the parameter is stored as
    a private data member for future retrieval by the getter member functions.
   */
  void init();
  BossClassAd::BossClassAd* clad_; ///< The BossClassAd that is created from JDL
  string hostname_; 
  string username_; 
  string password_; 
  string database_; 
  unsigned int port_;
  string unixSocket_;
  long clientFlag_;
};
#endif
