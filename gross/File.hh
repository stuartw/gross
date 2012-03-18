#ifndef FILE_H
#define FILE_H
#include "Log.hh"


//! Class to represent a file on the local file system.
/*!
  Handles full path or relative path file names.
  Can perform common tasks associated with local files, such as
  checking for existence, reading/saving contents of file, changing permissions, etc.
*/
class File {
public:
  /*! 
    \param fullHandle can be full (or relative) path (eg "/home/hugh/test.exe") or 
    just file name alone if it is in local directory (eg "test.exe").
    In the case of the latter, then location will be automatically set to "./"
    \sa File(string location, string name)
  */
  File(string fullHandle);
  /*!
    \param location is the full (or relative) path of the file (eg "/home/hugh" or "../hugh")
    \param name is the file's name (eg "test.exe")
    \sa File(string fullHandle)
  */
  File(string location, string name);
  void name(string name);   ///< Sets file name
  void location(string location);   ///< Sets file location (i.e. path)
  void fullHandle(string fullHandle);  ///< Sets file fullHandle (i.e. path + name)  
  /*!
    \retval 1 if file exists
    \retval 0 if file does not exist
  */
  int exists() const;   ///< Checks to see if file exists
  int save(string aString) const; ///< Saves string as contents of the file.
  int makeExec() const;  ///< Changes the file permissions to make it executable.
  int remove() const; ///< Removes file from file system
  const string contents() const;   ///< Reads contents of file
  const string name() const;   ///< Getter for name
  const string location() const;   ///< Getter for location (i.e. path)
  const string fullHandle() const;   ///< Getter for fullHandle (i.e. path + name)
private:
  string fullHandle_; ///< Name + location (eg "/home/hugh/test.exe" or "./test.exe") 
  string location_; ///< Location of the file (eg "/home/hugh" or "./")
  string name_; ///< Name of the file (eg "test.exe")

  File(const File&); ///< Not implemented
  File& operator=(const File&); ///< Not implemented
};

#endif
