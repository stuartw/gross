#include "Error.hh"

void fatal_error(const std::string where, const std::string what) {
  std::cerr << "GROSS fatal error in " << where <<std::endl;
  std::cerr << "ERROR: "<< what <<std::endl;
  std::exit(1);
}
void report_error(const std::string where, const std::string what) {
  std::cerr << "GROSS error in " << where <<std::endl;
  std::cerr << "ERROR: "<< what <<std::endl;
}
