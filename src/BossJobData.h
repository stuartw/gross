// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossJobData.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_JOB_DATA_H
#define BOSS_JOB_DATA_H

#include <map>
#include <vector>
#include <string>
#include <iostream>

#include "BossJobElement.h"

class BossJobData { 

public:

  typedef std::map< std::string , BossJobElement ,std::less<std::string> > Data;
  typedef Data::const_iterator const_iterator;
  typedef Data::iterator iterator;

  typedef std::map< std::string , std::string ,std::less<std::string> > Schema;
  typedef Schema::const_iterator schema_const_iterator;
  typedef Schema::iterator schema_iterator;

private:
  Data data_;
  Schema schema_;

public:

  BossJobData();

  BossJobData(std::string name, std::istream&);

  ~BossJobData();

  void add(const std::string table, const BossJobElement& e);
  void add(const std::string table, const std::string name, const std::string type);
  void add(const std::string table, const std::string name, const std::string type, const std::string value);
  void add(const std::string table, std::istream& schema);

  std::vector< std::pair<std::string,std::string> > splitSchema(std::istream& schema) const;
  std::vector< std::pair<std::string,std::string> > splitSchema(const std::string schema) const;

  std::string getSchema() const;
  bool existColumn(std::string table, std::string name) const;
  bool existName(std::string schema, std::string name) const;
  void assign(const std::string name, const std::string value);

  iterator find(std::string name);
  const_iterator find(std::string name) const;

  const BossJobElement& operator[](const std::string name) const;
  const BossJobElement& operator[](const_iterator i) const;
  BossJobElement& operator[](const std::string name);
  BossJobElement& operator[](iterator i);

  const_iterator begin() const { return data_.begin(); }
  const_iterator end() const { return data_.end(); }

};

#endif
