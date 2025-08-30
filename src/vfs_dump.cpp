// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#include "neon/vfs.hpp"
#include <fstream>
#include <stack>

namespace neon {

static void write_node(std::ofstream& out, const Node* n, const std::string& prefix){
  std::string path = (prefix == "/" ? "/" + n->name : prefix + (prefix=="/" ? "" : "/") + n->name);
  if (n->name == "/" || path == "//") path = "/";
  if (n->is_dir) {
    out << "DIR " << path << "\n";
    for (const auto& kv : n->children) {
      write_node(out, kv.second.get(), path);
    }
  } else {
    out << "FILE " << path << " " << n->content.size() << "\n";
    out << n->content << "\n";
  }
}

bool Vfs::SaveDump(const std::string& filename) const {
  std::ofstream out(filename, std::ios::out | std::ios::trunc);
  if (!out.is_open()) return false;
  out << "# NEON VFS dump v1\n";
  if (!root_) return false;
  if (root_->children.empty()) { out << "DIR /\n"; return true; }
  for (const auto& kv : root_->children) {
    write_node(out, kv.second.get(), "/");
  }
  return true;
}

bool Vfs::LoadDump(const std::string& filename) {
  std::ifstream in(filename);
  if (!in.is_open()) return false;
  root_ = std::make_unique<Node>(); root_->is_dir = true; root_->name = "/";
  std::string line;
  while (std::getline(in, line)) {
    if (line.rfind("#", 0) == 0 || line.empty()) continue;
    if (line.rfind("DIR ", 0) == 0) {
      std::string path = line.substr(4);
      Mkdir(path);
    } else if (line.rfind("FILE ", 0) == 0) {
      // Format: FILE <path> <len>\n<content>\n
      auto rest = line.substr(5);
      auto sp = rest.find(' ');
      if (sp == std::string::npos) continue;
      std::string path = rest.substr(0, sp);
      size_t len = std::stoul(rest.substr(sp+1));
      std::string content(len, '\0');
      in.read(&content[0], len);
      // consume trailing newline
      if (in.peek() == '\n') in.get();
      Write(path, content);
    }
  }
  return true;
}

} // namespace neon
