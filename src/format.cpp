#include "format.h"

#include <string>

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  long int t, h, m, s;
  string output;

  h = seconds / 3600;
  t = seconds % 3600;
  m = t / 60;
  s = t % 60;

  output = std::to_string(h) + string(":") + std::to_string(m) + string(":") +
           std::to_string(s);

  return output;
}
