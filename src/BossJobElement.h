// /////////////////////////////////////////////////////////////////////
// Program: BOSS
// Version: 2.1
// File:    BossJobElement.h
// Authors: Claudio Grandi (INFN BO), Alessandro Renzi (INFN BO)
// Date:    31/01/2002
// Note:
// /////////////////////////////////////////////////////////////////////

#ifndef BOSS_JOB_ELEMENT_H
#define BOSS_JOB_ELEMENT_H

#include <string>
#include <iostream>

class BossJobElement { 

  friend std::ostream& operator<<(std::ostream&, const BossJobElement&);

private:
  std::string name_;
  std::string type_;
  std::string value_;

public:
  BossJobElement() : name_(""), type_(""), value_("") {}

  BossJobElement(const std::string name, const std::string type) : 
    name_(name), type_(type), value_("") {}

  BossJobElement(const std::string name, const std::string type, const std::string value) : 
    name_(name), type_(type), value_(value) {}

  BossJobElement& initialize(const std::string name, const std::string type) {
    name_ = name;
    type_ = type;
    value_ = "";
    return *this;
  }

  ~BossJobElement() {}

  BossJobElement& operator=(const BossJobElement& e) {
    name_ = e.name_;
    type_ = e.type_;
    value_ = e.value_;
    return *this;
  }

  BossJobElement& operator=(const std::string value) {
    value_ = value;
    return *this;
  }

  BossJobElement& assign(const std::string value) {
    value_ = value;
    return *this;
  }

  bool operator< (const BossJobElement& e) const {
    return name_<e.name_;
  }

  bool operator== (const BossJobElement& e) const {
    return name_==e.name_;
  }

  std::string name() const { return name_; }
  std::string type() const { return type_; }
  std::string value() const { return value_; }

};

#endif


