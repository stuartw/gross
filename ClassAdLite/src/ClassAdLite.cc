// /////////////////////////////////////////////////////////////////////
//
//                           CladLite
//             Lightweight Classified Advertisement
//
//   File:ClassAdLite.cc
//   Version: 1.0
//   Author: Claudio Grandi (INFN Bologna)
//   Date: 3-May-2004
//   Change Log:
//
// /////////////////////////////////////////////////////////////////////

#include "ClassAdLite.h"
#include <fstream>
#include <sstream>

void CAL::clear(ClassAdLite& clad) {
  clad.clear();
}

int CAL::readFromFile(ClassAdLite& clad, std::string file) {
  clad.clear();
  return addFromFile(clad,file);
}

int CAL::read(ClassAdLite& clad, std::istream& is) {
  clad.clear();
  return add(clad,is);
}

int CAL::read(ClassAdLite& clad, std::string& s) {
  clad.clear();
  return add(clad,s);
}

int CAL::addFromFile(ClassAdLite& clad, std::string file) {
  int ret_val = 0;
  std::ifstream is(file.c_str());
  if ( is ) {
    ret_val = add(clad,is);
  } else {
    std::cerr << "ClassAdLite: Unable to open ClassAd file: " 
	      << file << std::endl; 
    ret_val = -1;
  }
  is.close();
  return ret_val;
}

int CAL::add(ClassAdLite& clad, std::string& s) {
  std::istringstream is(s);
  return CAL::add(clad, is);
}

int CAL::removeAttr(ClassAdLite& clad, std::string key) {
  CAL::ClassAdLite::iterator it = clad.find(key);
  if (it != clad.end())
    return CAL::removeAttr(clad,it);
  return 0;
}

int CAL::removeAttr(ClassAdLite& clad, CAL::ClassAdLite::iterator it) {
  clad.erase(it);
  return 0;
}

void CAL::writeToFile(const ClassAdLite& clad, std::string file) {
  std::ofstream of(file.c_str());
  CAL::write(clad,of);
  of.close();
}

void CAL::write(const ClassAdLite& clad, std::ostream& os) {
  if ( os ) {
    os << "[" << std::endl;
    for (CAL::ClassAdLite::const_iterator it = clad.begin(); it != clad.end(); ++it)
      os << (*it).first << " = " << (*it).second << ";" << std::endl;
    //      of << (*it).first << " = \"" << (*it).second << "\";" << std::endl;
    os << "]" << std::endl;
  }
}

// CAL::ClassAdLite::iterator CAL::find(ClassAdLite& clad, std::string key) {
//   CAL::ClassAdLite::iterator it;
//   for (it = clad.begin(); it != clad.end(); ++it) {
//     if ( key == it->first ) {
//       break;
//     }
//   }
//   return it;
// }

// CAL::ClassAdLite::const_iterator CAL::find(const ClassAdLite& clad, std::string key) {
//   CAL::ClassAdLite::const_iterator it;
//   for (it = clad.begin(); it != clad.end(); ++it) {
//     if ( key == it->first ) {
//       break;
//     }
//   }
//   return it;
// }

void CAL::lookup(const ClassAdLite& clad, std::string key, std::string& value) {
  CAL::ClassAdLite::const_iterator it = clad.find(key);
  if (it != clad.end()) {
    value=it->second;
    // DEBUG
    //std::cout << "lookup found " << key << "=" << value << std::endl;
    // END DEBUG
  }
}

void CAL::pop(ClassAdLite& clad, std::string key, std::string& value) {
  CAL::ClassAdLite::iterator it = clad.find(key);
  if (it != clad.end()) {
    value=it->second;
    // DEBUG
    //std::cout << "pop found " << key << "=" << value << std::endl;
    // END DEBUG
    CAL::removeAttr(clad,it);
  }
}
  
void CAL::removeOuterQuotes(std::string& s) {
  unsigned int i1 = 0;
  unsigned int i2 = s.size();
  while ( s[i1]==' ' )
    i1++;
  if ( s[i1] == '\"' ) {
    i1++;
    i2 = s.find_last_of("\"");
  }
  s = s.substr(i1,i2-i1);
}

std::vector<std::string> CAL::getList(std::string buf) {
  std::vector<std::string> ret_val;
  removeOuterQuotes(buf);
  unsigned int siz = buf.size();
  if ( siz>0 ) {
    std::string buffer = "";
    unsigned int start = buf.find_first_of('{',0);
    unsigned int stop = siz-1;
    if ( start >= siz ) {           // the attribute is not within {}
      buffer = buf;
    } else {
      stop = buf.find_last_of('}',siz-1);
      if ( stop >= siz ) {
	std::cerr << "ClassAdLite: found { but not } in list. "
		  << "Using buffer from { to the end" << std::endl;
	stop = siz-1;
      }
      buffer = buf.substr(start+1,stop-start-1);
    }
    unsigned int i = 0;
    unsigned int n = buffer.size();
    while (i < n ) {
      unsigned int j = buffer.find_first_of(',',i); // split by coma
      j = j<n ? j : n;
      std::string line = buffer.substr(i,j-i);
      if (line.size() > 0 )
	ret_val.push_back(line);
      i = j+1;
    }
  }
  return ret_val;
}

// Returns 0 if no error or number of errors found
int CAL::add(CAL::ClassAdLite& clad, std::istream& is) {
  int ret_val = 0;
  int sqrblevel = 0;
  while (is) {
    // find first valid character in stream
    char c;
    while( (is.get(c)) ) {
      // DEBUG
      // std::cout << "add -> " << c << std::endl;
      // END DEBUG
      // ignore ' ', tabs and CR before attr.
      if ( c == ' ' || c == '\n' || c == '\t' ) 
	continue;
      // ignore lines where the first char is #
      if ( c == '#' ) {
	while ( (is.get(c)) && c!='\n' ) {
	  // DEBUG
	  // std::cout << "add skipping -> " << c << std::endl;
	  // END DEBUG
	}
	continue;
      }
      // keep track of []
      if ( c == '[' ) {
	++sqrblevel;             
	continue;
      }
      if ( c == ']' ) {
	--sqrblevel;
	continue;
      }
      break;
    }
    // add next attribute
    is.unget();
    std::string attr = CAL::nextAttr(is);
    if ( attr.size() > 0 )
      ret_val += CAL::addAttr(clad,attr);
  }
  return ret_val;
}

std::string CAL::nextAttr(std::istream& is) {
  std::string ret_val = "";
  int sqrblevel = 0;
  int curlblevel = 0;
  int parlevel = 0;
  bool insquote = false;
  bool indquote = false;
  char c;
  while ( (is.get(c)) &&  (c != ';' ||
			   sqrblevel != 0 ||
			   curlblevel != 0 ||
			   parlevel != 0 ||
			   insquote ||
			   indquote ) ) {
    switch (c) {
    case '[':
      ++sqrblevel;
      break;
    case ']':
      --sqrblevel;
      break;
    case '{':
      ++curlblevel;
      break;
    case '}':
      --curlblevel;
      break;
    case '(':
      ++parlevel;
      break;
    case ')':
      --parlevel;
      break;
    case '\'':
      insquote = !insquote;
      break;
    case '"':
      indquote = !indquote;
      break;
    } 
    ret_val += c;
    // DEBUG
    // std::cout << "nextAttr -> " << c << " mask: "
    // 	      << sqrblevel << curlblevel << parlevel << insquote << indquote 
    // 	      << std::endl;
    // END DEBUG
  }
  // DEBUG
  // std::cout << "nextAttr returning \"" << ret_val << "\"" << std::endl;
  // END DEBUG
  return ret_val;
}

// returns 0 if no error, 1 if an error is found
int CAL::addAttr(ClassAdLite& clad, const std::string& attr) {
  // DEBUG
  // std::cout << "addAttr --> " << attr << std::endl;
  // END DEBUG
  std::string key = "";
  unsigned int i = 0;
  unsigned int attrsiz = attr.size();
  while ( i<attrsiz && attr[i] != '=' ) {
    if ( attr[i] == ' ' || attr[i] == '\n' || attr[i] == '\t') {
      ++i;
      continue;
    }	
    // DEBUG
    // std::cout << "addAttr (key) --> " << attr[i] << std::endl;
    // END DEBUG
   key+= attr[i++];
  }
  std::string val = "";
  int sqrblevel = 0;
  int curlblevel = 0;
  int parlevel = 0;
  bool insquote = false;
  bool indquote = false;
  while ( ++i<attrsiz && ( attr[i] != ';' ||
			   sqrblevel !=0 ||
			   curlblevel != 0 ||
			   parlevel != 0 ||
			   insquote ||
			   indquote ) ) {
    // DEBUG
    // std::cout <<  "addAttr (val) --> " << attr[i] << std::endl;
    // END DEBUG
    if ( ( attr[i] == ' ' || attr[i] == '\n' || attr[i] == '\t' ) &&
	 ( sqrblevel == 0 && curlblevel == 0 && parlevel == 0 && 
	   !insquote && !indquote ) )
      continue;
    switch (attr[i]) {
    case '[':
      ++sqrblevel;
      break;
    case ']':
      --sqrblevel;
      break;
    case '{':
      ++curlblevel;
      break;
    case '}':
      --curlblevel;
      break;
    case '(':
      ++parlevel;
      break;
    case ')':
      --parlevel;
      break;
    case '\'':
      insquote = !insquote;
      break;
    case '"':
      indquote = !indquote;
      break;
    }
    val+= attr[i];
  }
  if (key.size()>0)
    clad[key] = val;
  // DEBUG
  // std::cout << "addAttr clad[" << key << "] = " << val << std::endl;
  // END DEBUG
  return 0;
}
