#ifndef STRINGSPECIAL_H
#define STRINGSPECIAL_H

#include <iostream>
#include <algorithm>
#include <string>

using std::string;

/*! \brief Some helpful functions
  Trying not to pollute global namespace for these...
*/

namespace stringSpecial {
  bool nocase_compare(char c1, char c2);
  bool stringSearch(string inputS, string keyword);
}

#endif
