// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "neon/crypto.hpp"
#include "neon/vfs.hpp"

namespace neon {

inline std::string BlobHash(const std::string& content){
  // Git-like prefix for determinism
  std::string prefix = "blob " + std::to_string(content.size()) + '\0';
  Sha256 h; h.update(prefix.data(), prefix.size()); h.update(content.data(), content.size());
  return Sha256::hex(h.digest());
}

inline bool IsDir(const Vfs& v, const std::string& path){
  bool is_dir=false; std::size_t sz=0;
  return v.Stat(path, is_dir, sz) && is_dir;
}

inline std::string MerklePath(Vfs& v, const std::string& path){
  bool is_dir=false; std::size_t sz=0;
  if (!v.Stat(path, is_dir, sz)) return {};
  if (!is_dir){
    auto s = v.Read(path); if (!s) return {};
    return BlobHash(*s);
  }
  // Directory: hash of sorted child entries: "<name>\t<type>\t<hash>\n"
  auto children = v.List(path);
  std::vector<std::string> lines;
  for (auto& name : children){
    std::string child = (path=="/") ? ("/"+name) : (path + "/" + name);
    auto h = MerklePath(v, child);
    std::string type = IsDir(v, child) ? "dir" : "file";
    lines.push_back(name + "\t" + type + "\t" + h + "\n");
  }
  std::sort(lines.begin(), lines.end());
  std::string acc = "tree " + std::to_string(lines.size()) + '\0';
  for (auto& L : lines) acc += L;
  Sha256 hh; hh.update(acc.data(), acc.size());
  return Sha256::hex(hh.digest());
}

inline std::string MerkleRoot(Vfs& v){ return MerklePath(v, "/"); }

} // namespace neon


inline std::string MerklePathMeta(Vfs& v, const std::string& path){
  bool is_dir=false; std::size_t sz=0;
  if (!v.Stat(path, is_dir, sz)) return {};
  Vfs::Meta mt{}; v.GetMeta(path, mt);
  if (!is_dir){
    auto s = v.Read(path); if (!s) return {};
    std::string prefix = "blob " + std::to_string(s->size()) + '\0';
    Sha256 h; h.update(prefix.data(), prefix.size()); h.update(s->data(), s->size());
    auto d = Sha256::hex(h.digest());
    std::string line = "meta " + std::to_string(mt.mode) + " " + std::to_string(mt.mtime) + " " + std::to_string(mt.ctime) + "\n";
    Sha256 m; m.update(line.data(), line.size());
    std::string meta = Sha256::hex(m.digest());
    Sha256 tot; auto bin = d + meta; tot.update(bin.data(), bin.size());
    return Sha256::hex(tot.digest());
  }
  auto children = v.List(path);
  std::vector<std::string> lines;
  for (auto& name : children){
    std::string child = (path=="/") ? ("/"+name) : (path + "/" + name);
    auto h = MerklePathMeta(v, child);
    std::string type = IsDir(v, child) ? "dir" : "file";
    lines.push_back(name + "\t" + type + "\t" + h + "\n");
  }
  std::sort(lines.begin(), lines.end());
  std::string acc = "tree " + std::to_string(lines.size()) + '\0';
  for (auto& L : lines) acc += L;
  Sha256 hh; hh.update(acc.data(), acc.size());
  return Sha256::hex(hh.digest());
}


inline std::string MerklePathMetaX(Vfs& v, const std::string& path){
  bool is_dir=false; std::size_t sz=0;
  if (!v.Stat(path, is_dir, sz)) return {};
  Vfs::Meta mt{}; v.GetMeta(path, mt);
  // Serialize xattrs (sorted)
  std::vector<std::string> xa = v.XAttrList(path);
  std::string xastr;
  for (auto& k : xa){
    std::string val; v.XAttrGet(path, k, val);
    xastr += k; xastr += "="; xastr += val; xastr += "\n";
  }
  if (!is_dir){
    auto s = v.Read(path); if (!s) return {};
    std::string prefix = "blob " + std::to_string(s->size()) + '\0';
    Sha256 h; h.update(prefix.data(), prefix.size()); h.update(s->data(), s->size()); auto d = Sha256::hex(h.digest());
    std::string meta = "meta " + std::to_string(mt.mode) + " " + std::to_string(mt.mtime) + " " + std::to_string(mt.ctime) + "\n";
    Sha256 tot; auto bin = d + meta + xastr; tot.update(bin.data(), bin.size()); return Sha256::hex(tot.digest());
  }
  auto children = v.List(path);
  std::vector<std::string> lines;
  for (auto& name : children){
    std::string child = (path=="/") ? ("/"+name) : (path + "/" + name);
    auto h = MerklePathMetaX(v, child);
    std::string type = IsDir(v, child) ? "dir" : "file";
    lines.push_back(name + "\t" + type + "\t" + h + "\n");
  }
  std::sort(lines.begin(), lines.end());
  std::string acc = "tree " + std::to_string(lines.size()) + '\0' + xastr;
  Sha256 hh; hh.update(acc.data(), acc.size()); return Sha256::hex(hh.digest());
}
