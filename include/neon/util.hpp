// SPDX-License-Identifier: MIT
#pragma once
#include <string>
#include <vector>

namespace neon {

std::vector<std::string> Split(const std::string& s, char delim);
std::string Join(const std::vector<std::string>& parts, const std::string& delim);

}  // namespace neon
