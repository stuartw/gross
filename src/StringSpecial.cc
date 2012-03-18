#include "StringSpecial.hh"
namespace stringSpecial {
  bool nocase_compare(char c1, char c2) {
    return toupper(c1) == toupper(c2);
  } 
  bool stringSearch(string inputS, string keyword) {
    string::iterator pos = search(inputS.begin(), inputS.end(), 
				  keyword.begin(), keyword.end(), 
				  nocase_compare);
    if(pos!=inputS.end()) return true;
    return false;
  }
  string trim(string input) {
    int a = input.find_first_not_of(" ");
    if (a==string::npos) return "";  
    return input.substr(a);
  }
}
