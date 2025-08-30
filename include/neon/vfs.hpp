// SPDX-License-Identifier: MIT
#pragma once
#include <unordered_map>
#include <list>
#include <cstdint>
#include <chrono>
#include <functional>
#include <mutex>
#include "neon/export.hpp"
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace neon {

enum class Perm : uint8_t { Read=1, Write=2, Exec=4 };

inline Perm operator|(Perm a, Perm b) {
  return static_cast<Perm>(static_cast<int>(a) | static_cast<int>(b));
}

struct Node {
  bool is_dir = true;
  std::string name;
  std::string content;
  std::map<std::string, std::unique_ptr<Node>> children;
};


class NEON_API Vfs {
 public:
  struct Meta {  uint32_t mode = 0o100644; uint64_t ctime=0; uint64_t mtime=0;  bool immutable = false; };
  // Metadata API
  bool SetMode(std::string_view path, uint32_t mode);
  bool Utime(std::string_view path, uint64_t epoch);
  bool GetMeta(std::string_view path, Meta& out) const;
  // Cache public API
  std::string CacheStats() const; bool CacheOn(bool on) const; bool CacheSize(size_t bytes) const; void CacheClear() const;
  // Quota API
  bool QuotaSet(std::string_view prefix, size_t bytes);
  bool QuotaDel(std::string_view prefix);
  std::string QuotaReport() const;
  // Locks API
  bool Lock(std::string_view path, uint64_t ttl_ms);
  bool Unlock(std::string_view path);
  // xattr API
  bool XAttrSet(std::string_view path, std::string_view key, std::string_view value);
  bool XAttrGet(std::string_view path, std::string_view key, std::string& out) const;
  std::vector<std::string> XAttrList(std::string_view path) const;
  bool XAttrRm(std::string_view path, std::string_view key);
  // Umask/default modes
  void SetUmask(uint32_t m);
  uint32_t Umask() const;
  void SetDefaultModes(uint32_t file_mode, uint32_t dir_mode);
  // New I/O helpers
  bool SetImmutable(std::string_view path, bool imm);
  bool GetImmutable(std::string_view path) const;
  bool Remove(std::string_view path, bool recursive=false);

  bool Append(std::string_view path, std::string_view content);
  bool Truncate(std::string_view path, std::size_t new_size);
  std::optional<std::string> ReadRange(std::string_view path, std::size_t offset, std::size_t len) const;

 public:
  bool Copy(std::string_view from, std::string_view to, bool recursive=true);
  bool Stat(std::string_view path, bool& is_dir, std::size_t& size) const;

 public:
  // Walk every path (callback receives path, is_dir, size)
  void Walk(const std::function<void(const std::string&, bool, std::size_t)>& cb) const;
  // Produce a JSON with basic integrity info
  std::string FsckJson() const;

 public:
  bool Remove(std::string_view path, bool recursive);
  bool Move(std::string_view from, std::string_view to);

 public:
  // --- Transactions ---
  bool BeginTx();
  bool CommitTx();
  bool RollbackTx();
  bool TxActive() const;
  // Dump/Load to string (uses file helpers under the hood)
  bool SaveDumpToString(std::string& out) const;
  bool LoadDumpFromString(const std::string& in);

 public:
  void SetQuota(std::size_t bytes);
  std::size_t Quota() const;
  std::size_t UsedBytes() const;

 public:
  void Seal(bool on); bool Sealed() const;

 public:
  // Save/Load a deterministic dump format (no external deps)
  bool SaveDump(const std::string& filename) const;
  bool LoadDump(const std::string& filename);

 public:

 public:
  Vfs();
  bool Mkdir(std::string_view path);
  bool Touch(std::string_view path);
  bool Write(std::string_view path, std::string_view content);
  std::optional<std::string> Read(std::string_view path) const;
  std::vector<std::string> List(std::string_view path) const;
  bool Exists(std::string_view path) const;

 public:
  static std::string NormalizePath(std::string_view path);

 private:
  bool is_immutable_(const std::string& norm) const;
  uint32_t umask_ = 0022; uint32_t def_file_mode_ = 0100644; uint32_t def_dir_mode_ = 040755;
  bool can_write_(const std::string& norm) const;
  void cache_clear_() const; void cache_touch_(const std::string& k) const; std::optional<std::string> cache_get_(const std::string& k) const; void cache_put_(const std::string& k, const std::string& v) const; void cache_invalidate_(const std::string& k) const;
  std::string best_quota_prefix_(const std::string& norm) const;
  bool quota_reserve_(const std::string& norm, long long delta);
  void quota_apply_(const std::string& norm, long long delta);
  bool locked_(const std::string& norm) const;
  void clean_locks_() const;
  std::unordered_map<std::string, Meta> meta_;
  /* CACHE */ mutable bool cache_on_ = true; mutable size_t cache_capacity_ = (1<<20); mutable size_t cache_size_ = 0; mutable std::list<std::string> lru_; mutable std::unordered_map<std::string, std::pair<std::string, std::list<std::string>::iterator>> cache_; mutable uint64_t cache_hits_=0, cache_misses_=0;
  /* QUOTAS */ std::unordered_map<std::string,size_t> quota_limit_;
  std::unordered_map<std::string,size_t> quota_used_;
  std::unordered_map<std::string,size_t> path_sizes_;
  /* LOCKS */ std::unordered_map<std::string, uint64_t> locks_ms_;
  // GC helpers
  size_t PruneOrphans();
  /* XATTRS */ std::unordered_map<std::string, std::map<std::string,std::string>> xattrs_;
  void touch_meta_(const std::string& path, bool is_new);
  bool tx_active_ = false;
  std::string tx_snapshot_;
  std::size_t quota_bytes_ = 0;
  std::size_t calc_used_(const struct Node* n) const;
  bool sealed_ = false;
  mutable std::mutex mu_;

  std::unique_ptr<Node> root_;
  Node* Traverse(std::string_view path) const;
  Node* EnsureDir(std::string_view path);
};

}  // namespace neon
