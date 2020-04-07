#include "format.h"
#include <iomanip>
#include <sstream>
#include <string>

using std::setfill;
using std::setw;
using std::string;

// DONE: Complete this helper function
string Format::ElapsedTime(long seconds) {
  int hh, mm, ss;
  std::stringstream stream;
  hh = seconds / 3600;
  mm = (seconds % 3600) / 60;
  ss = (seconds % 3600) % 60;
  stream << setw(2) << setfill('0') << hh << ":" << setw(2) << mm << ":"
         << setw(2) << ss;
  return stream.str();
}

// Convert kilobytes to megabytes ronded to one significant digit
string Format::kB2MB(long kb) {
  std::stringstream stream;
  stream << setw(6) << (kb / 100) * 0.1;
  return stream.str();
}