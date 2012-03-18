#ifndef LOCALDB_H
#define LOCALDB_H
#include "Log.hh"

#include <mysql.h>
class DbSpec;

//! Singleton class for interfacing to MySQL database
/*!
  This class can be used to establish connection to a MySQL database and then either
  use its member functions to perform some common standardised queries on the database, 
  or alternatively get the MYSQL* connection itself to interface to the database using
  the MySQL API directly (though in principle, it would be neater to have all required 
  query functions built into this class as member functions).
  It is implemented as a singleton to ensure only a single connection is opened to the database, 
  and to ensure global visibility for all client classes.
  
  \todo Make a base class for a generic MySQL database connection and then have derived
  classes for LocalDb, GroupDb, RefDb, etc.
  \todo Make public member function names more appropriate (eg tableRead->colRead, etc).
*/
class LocalDb {
public:
  ~LocalDb(); ///< Disconnects from Db
  /*!
    \brief Initialises object and calls connect member function
    \param aDbSpec is a DbSpec object that contains connection details for this particular database instance
    \sa connect()
  */
  int init(DbSpec* aDbSpec);
  /*! 
    \brief Returns connection handle for db for MySQL API
    \return MYSQL* see the MYSQL C API documentation.
  */
  MYSQL* connection() {return current_connection_;};
  static  LocalDb* instance(); ///< Returns instance of the singleton

  /*!
    \brief Read value(s) of a field from a row of a table given a particular selection.

    Parameters translate into the following SQL command: SELECT aKey FROM aTable WHERE aSelection; \n
    Multiple values from the query are returned via the aValues vector.
    \param aTable is the table name
    \param aKey is the field for which the value(s) are required. 
    Must be single valued (an error will be returned if multiple fields are returned from the query)
    \param aSelection is the SQL selection (eg ID=100)
    \param aValues is the vector of results that is filled
    \retval 0 for success
    \retval 1 for failure
   */
  int tableRead(string aTable, string aKey, string aSelection, vector<string>& aValues);
  /*!
    \brief Inserts a new row into a table.

    Parameters translate into the following SQL command: INSERT INTO aTable (aKeys) VALUES aValues; \n
    \param aTable is the table name
    \param aKeys is either single or multiple field names
    \param aValues is the value of each field. Must be in same order as aKeys
    \retval 0 for success
    \retval 1 for failure
   */
  int tableSave(string aTable, string aKeys, string aValues) const;
  /*!
    \brief Update a field in a table to a new value, given an optional selection criterion.

    Parameters translate into the following SQL command: UPDATE aTable SET aKey=aValue [WHERE aSelection]; \n
    \param aTable is the table name
    \param aKey is the field name. It must be single valued
    \param aValue is the new value to set the field to. It must be single valued.
    \param aSelection is the optional selection criteria.
    \retval 0 for success
    \retval 1 for failure
   */
  int tableUpdate(string aTable, string aKey, string aValue, string aSelection = "") const;
  /*!
    \brief Deletes rows from a table for a particular selection.

    Parameters translate into the following SQL command: DELETE FROM aTable WHERE aSelection; \n
    \param aTable is the table name
    \param aSelection is the selection criteria.
    \retval 0 for success
    \retval 1 for failure
   */
  int tableDelete(string aTable, string aSelection) const;
  /*!
    \brief Returns the maximum (integer) value of a field in a table.

    Parameters translate into the following SQL command: SELECT MAX (aCol) FROM aTable; \n
    \param aTable is the table name
    \param aCol is the field/column name.
    \retval 0 for failure
    \retval >0 is the max value found. Note it is converted to an integer, thus is only suited to integer values.
   */
  int tableCreate(string aTable, vector<string> aFields) const;
  /*!
   \brief Creates a table in the database consistent with list of strings 'field_name TYPE(SIZE)' in the vector.
   \param aTable is the table name to be created.
   \param aFields is the list of fields names/types.
   \retval 0 for success
   \retval 1 for failure
 */
  int LocalDb::maxCol(string aTable,string aCol) const;
  /*!
    \brief Converts a string containing characters that MYSQL requires escaping into a legal SQL string

    \param aString is the unescaped string requiring conversion
    \retval is the escaped string (empty if an error occurred)
   */
  string LocalDb::escapeString(const string& aString) const;
private:
  LocalDb(); ///< Singleton implementation, thus ctor is private.
  int connect(); ///< Makes Db connection
  int disconnect(); ///< Disconnects from Db
  MYSQL* current_connection_; ///< MYSQL API database connection handle
  DbSpec* dbSpec_; ///< DbSpec object that describes connection details for this particular LocalDb object
  static LocalDb* instance_; ///< Singleton instance
};
#endif
