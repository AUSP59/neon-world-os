// SPDX-License-Identifier: MIT
#include "neon/util.hpp"
#include <sstream>

namespace neon {

std::vector<std::string> Split(const std::string& s, char delim) {
  std::vector<std::string> out;
  std::string cur;
  for (char c : s) {
    if (c == delim) { if (!cur.empty()) out.push_back(cur); cur.clear(); }
    else { cur.push_back(c); }
  }
  if (!cur.empty()) out.push_back(cur);
  return out;
}

std::string Join(const std::vector<std::string>& parts, const std::string& delim) {
  std::ostringstream oss;
  for (size_t i=0;i<parts.size();++i) {
    if (i) oss << delim;
    oss << parts[i];
  }
  return oss.str();
}

}  // namespace neon
