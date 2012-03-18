#ifndef CLADLOOKUP_H
#define CLADLOOKUP_H
#include "Log.hh"

#include "ClassAdLite.h"
#include <map>
using std::map;

//! Class used to lookup parameters from a ClassAd
/*!
  This class is used to look up parameter values from a ClassAd.
  Not so hard you might think, but consider the following functionality that this 
  class satisfies: 
  \li the class can cope with parameter values expressed in the ClassAd as single valued or multi-valued (ie a list)
  \li the class will keep track of which parameters are read by any external clients, allowing the ability
  to dump unread parameters from the classAd. This is useful when some unknown parameters from the ClassAd 
  need to be passed through transparently to another system - eg BOSS in GROSS's case.
  \li any parameter can be looked up - a simple key string is all a client requires.
  
  \todo Obsolete DbSpec class by using this more general class instead
*/
class CladLookup {
public:
  /*!
    \brief Ctor sets default parameters, creates classad object from ClassAd
    \param aClad is a string containing the ClassAd text
   */
  CladLookup(const string& aClad);
  ~CladLookup(); ///< Dtor deletes all private members and objects created on heap.
  /*!
    \brief Looks up key and returns a string containing value for that key.
    
    For the case where the value of a key is single-valued (ie not a list).

    The key is looked up using the lookup() function. 
    A string with the value found in the classAd for that key is returned. 

    \param aKey is the key to be looked up in the ClassAd
    \retval <const string> string containing value or null if no key found in ClassAd
    \sa lookup()
   */
  const string read(const string& aKey);
  /*!
    \brief Looks up key and adds to vector of strings the values for that key.

    For the case where the value of a key might be a list of values.
    
    The key is looked up using the lookup() function. 
    If the value for the key is actually a list, then each string will be pushed into the vector.

    As a list of values is returned in jdl format from lookup (and indeed from the classAd lookup function), this needs 
    to be split up into a vector of separate strings for each item in the list. This is done by string2vec().

    \param aKey is the key to be looked up in the ClassAd
    \retval <int> 0 for successfully found value for key, 1 for failed to find key value.
    \sa lookup()
    \sa string2vec()
   */
  int read(const string aKey, vector<string>& aVec);
  /*!
    \brief Returns a dump of all unread parameters from the ClassAd in jdl format.
    
    Each time a parameter is read from the ClassAd, it is removed from that ClassAd. 
    When this function is called, all unread parameters from the ClassAd are read 
    and returned. Clearly, once called, the ClassAd will empty itself! 

    As the return must be in jdl format, the action of the ClassAd read functions, which
    clean up the returned values, needs to be reversed - i.e. the jdl format is re-encoded.

    Note that if jdlDump_ has been set by an external client via jdlDump(aJdlDump), then this is used as the return and 
    not the ClassAd itself.

    \sa jdlDump(string aJdlDump)
   */
  const string jdlDump() const;
  /*! 
    \brief Force jdlDump() to return a particular value
    
    There is sometimes the requirement to force the jdlDump() function to return a known value.
    This can be set here. 

    Example of use from GROSS: when a task is read from a database, some of its parameters are read from 
    a ClassAd stored on the database, while others are read from tables within the database
    (this is for consistency and to enable changes to be made via the database). This is in contrast to when a new Task is
    created where parameters are read exclusively from the ClassAd. Thus not all parameters
    are read (and hence removed) from the ClassAd in the DbTask's case. 
    This would lead to incorrect parameters being passed transparently through to BOSS.
    
    \param aJdlDump is the ClassAd Dump value required to be set, given in jdl format.
    \sa jdlDump()
   */
  void jdlDump(const string aJdlDump);
  /*!
  \brief Pretty print function for CladLookup
  */
  friend ostream& operator<<(ostream&, const CladLookup&);
private:
  //BossClassAd::BossClassAd* clad_; ///< The (Boss)ClassAd to be looked up.
  CAL::ClassAdLite* clad_;
  void string2vec(const string& cladline, vector<string>& strings) const; ///< Converts a single string list into a vector of separated strings

  //General map for all types
  /*!
    \brief Looks up parameters in the ClassAd
    When a parameter is read from a ClassAd, it is automatically deleted.
    To allow multiple reads, once it has been read once from ClassAd, the parameter is stored in a map for future lookups.
  */  
  const string lookup(const string& aKey);
  map<string, string> cladMap_; ///< General map containing key and value pairs for all looked up parameters

  string jdlDump_; ///< Explicit jdlDump_ value returned by jdlDump() function if set by an external client.

  //No implementation
  CladLookup(const CladLookup&); ///< Not implemented
  CladLookup& operator=(const CladLookup&); ///< Not implemented
};

#endif
