// SPDX-License-Identifier: MIT
#include "neon/vfs.hpp"
#include <sstream>










namespace neon {

void Vfs::cache_clear_() const {
  cache_.clear(); lru_.clear(); cache_size_ = 0; cache_hits_=cache_misses_=0;
}
void Vfs::cache_touch_(const std::string& k) const {
  auto it = cache_.find(k); if (it==cache_.end()) return;
  lru_.erase(it->second.second);
  lru_.push_front(k);
  it->second.second = lru_.begin();
}
std::optional<std::string> Vfs::cache_get_(const std::string& k) const {
  if (!cache_on_) return std::nullopt;
  auto it = cache_.find(k);
  if (it==cache_.end()){ ++cache_misses_; return std::nullopt; }
  cache_touch_(k); ++cache_hits_;
  return it->second.first;
}
void Vfs::cache_put_(const std::string& k, const std::string& v) const {
  if (!cache_on_) return;
  auto it = cache_.find(k);
  if (it!=cache_.end()){
    cache_size_ -= it->second.first.size();
    it->second.first = v;
    cache_touch_(k);
    cache_size_ += v.size();
  } else {
    lru_.push_front(k);
    cache_[k] = {v, lru_.begin()};
    cache_size_ += v.size();
  }
  while (cache_size_ > cache_capacity_ && !lru_.empty()){
    auto last = lru_.back(); lru_.pop_back();
    auto it2 = cache_.find(last);
    if (it2!=cache_.end()){ cache_size_ -= it2->second.first.size(); cache_.erase(it2); }
  }
}
void Vfs::cache_invalidate_(const std::string& k) const {
  auto it = cache_.find(k);
  if (it!=cache_.end()){ cache_size_ -= it->second.first.size(); lru_.erase(it->second.second); cache_.erase(it); }
}
std::string Vfs::CacheStats() const{
  std::ostringstream o; o << "{ \"on\": " << (cache_on_? "true":"false") << ", \"capacity\": " << cache_capacity_ << ", \"size\": " << cache_size_ << ", \"hits\": " << cache_hits_ << ", \"misses\": " << cache_misses_ << " }"; return o.str();
}
bool Vfs::CacheOn(bool on) const { cache_on_ = on; return true; }
bool Vfs::CacheSize(size_t bytes) const { cache_capacity_ = bytes; return true; }
void Vfs::CacheClear() const { cache_clear_(); }
namespace neon {

std::string Vfs::best_quota_prefix_(const std::string& norm) const{
  std::string best; size_t best_len = 0;
  for (auto const& kv : quota_limit_){
    const std::string& p = kv.first;
    if (p.size()<=norm.size() && norm.rfind(p, 0)==0){
      if (p.size() > best_len){ best = p; best_len = p.size(); }
    }
  }
  return best;
}

bool Vfs::quota_reserve_(const std::string& norm, long long delta){
  if (delta <= 0) quota_apply_(normR, -(long long)old); path_sizes_.erase(normR); cache_invalidate_(normR); return true;
  std::string qp = best_quota_prefix_(norm);
  if (qp.empty()) return true;
  size_t lim = quota_limit_[qp];
  size_t used = quota_used_[qp];
  if (used + (size_t)delta > lim) return false;
  return true;
}
void Vfs::quota_apply_(const std::string& norm, long long delta){
  std::string qp = best_quota_prefix_(norm);
  if (qp.empty()) return;
  long long u = (long long)quota_used_[qp] + delta;
  if (u < 0) u = 0;
  quota_used_[qp] = (size_t)u;
}

bool Vfs::QuotaSet(std::string_view prefix, size_t bytes){
  std::string p = NormalizePath(prefix);
  quota_limit_[p] = bytes;
  // recompute used for that prefix
  size_t sum = 0;
  Walk([&](const std::string& path, bool is_dir, std::size_t sz){
    if (!is_dir && path.rfind(p,0)==0){ sum += sz; path_sizes_[path] = sz; }
  });
  quota_used_[p] = sum;
  return true;
}
bool Vfs::QuotaDel(std::string_view prefix){
  std::string p = NormalizePath(prefix);
  quota_limit_.erase(p);
  quota_used_.erase(p);
  return true;
}
std::string Vfs::QuotaReport() const{
  std::ostringstream o; o << "{";
  bool first=true;
  for (auto const& kv : quota_limit_){
    if (!first) o << ","; first=false;
    auto it = quota_used_.find(kv.first);
    size_t u = (it==quota_used_.end()?0:it->second);
    o << "\"" << kv.first << "\": {\"limit\": " << kv.second << ", \"used\": " << u << "}";
  }
  o << "}"; return o.str();
}
namespace neon {

static uint64_t now_ms_(){
  using namespace std::chrono;
  return (uint64_t)duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void Vfs::clean_locks_() const {
  uint64_t now = now_ms_();
  std::vector<std::string> dead;
  for (auto const& kv : locks_ms_) if (kv.second <= now) dead.push_back(kv.first);
  for (auto& k : dead) const_cast<std::unordered_map<std::string,uint64_t>&>(locks_ms_).erase(k);
}

bool Vfs::locked_(const std::string& norm) const {
  clean_locks_();
  auto it = locks_ms_.find(norm);
  return it != locks_ms_.end();
}

bool Vfs::Lock(std::string_view path, uint64_t ttl_ms){
  std::string norm = NormalizePath(path);
  clean_locks_();
  if (locked_(norm)) return false;
  locks_ms_[norm] = now_ms_() + ttl_ms;
  return true;
}
bool Vfs::Unlock(std::string_view path){
  std::string norm = NormalizePath(path);
  clean_locks_();
  return locks_ms_.erase(norm) > 0;
}
namespace neon {

size_t Vfs::PruneOrphans(){
  // Remove meta_/xattrs_ entries whose paths no longer exist
  std::vector<std::string> dead;
  for (auto const& kv : meta_){
    if (!Exists(kv.first)) dead.push_back(kv.first);
  }
  for (auto &k : dead) meta_.erase(k);
  std::vector<std::string> deadx;
  for (auto const& kv : xattrs_){
    if (!Exists(kv.first)) deadx.push_back(kv.first);
  }
  for (auto &k : deadx) xattrs_.erase(k);
  return dead.size() + deadx.size();
}
namespace neon {

bool Vfs::is_immutable_(const std::string& norm) const {
  auto it = meta_.find(norm);
  if (it==meta_.end()) return false;
  return it->second.immutable;
}
bool Vfs::SetImmutable(std::string_view path, bool imm){
  std::string norm = NormalizePath(path);
  if (!Exists(norm)) return false;
  auto &m = meta_[norm]; m.immutable = imm; touch_meta_(norm, false); path_sizes_[norm] += content.size(); quota_apply_(norm, delta); cache_invalidate_(norm); return true;
}
bool Vfs::GetImmutable(std::string_view path) const{
  std::string norm = NormalizePath(path);
  auto it = meta_.find(norm); if (it==meta_.end()) return false;
  return it->second.immutable;
}
namespace neon {

bool Vfs::XAttrSet(std::string_view path, std::string_view key, std::string_view value){
  std::string p = NormalizePath(path);
  if (!Exists(p)) return false;
  xattrs_[p][std::string(key)] = std::string(value);
  touch_meta_(p, false);
  return true;
}
bool Vfs::XAttrGet(std::string_view path, std::string_view key, std::string& out) const{
  std::string p = NormalizePath(path);
  auto it = xattrs_.find(p);
  if (it==xattrs_.end()) return false;
  auto it2 = it->second.find(std::string(key));
  if (it2==it->second.end()) return false;
  out = it2->second; return true;
}
std::vector<std::string> Vfs::XAttrList(std::string_view path) const{
  std::string p = NormalizePath(path);
  std::vector<std::string> ks;
  auto it = xattrs_.find(p);
  if (it==xattrs_.end()) return ks;
  for (auto const& kv : it->second) ks.push_back(kv.first);
  std::sort(ks.begin(), ks.end());
  return ks;
}
bool Vfs::XAttrRm(std::string_view path, std::string_view key){
  std::string p = NormalizePath(path);
  auto it = xattrs_.find(p);
  if (it==xattrs_.end()) return false;
  auto k = it->second.find(std::string(key));
  if (k==it->second.end()) return false;
  it->second.erase(k);
  touch_meta_(p, false);
  return true;
}


void Vfs::SetUmask(uint32_t m){ umask_ = m & 0777; }
uint32_t Vfs::Umask() const { return umask_; }
void Vfs::SetDefaultModes(uint32_t fm, uint32_t dm){ def_file_mode_ = fm; def_dir_mode_ = dm; }

bool Vfs::can_write_(const std::string& norm) const {
#ifdef NEON_ENFORCE_PERMS
  auto it = meta_.find(norm);
  if (it==meta_.end()) return true; // new files allowed
  uint32_t mode = it->second.mode;
  return (mode & 0200) != 0; // user-write bit
#else
  (void)norm; return true;
#endif
}
namespace neon {

static uint64_t now_epoch_(){
  using namespace std::chrono;
  return (uint64_t)duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

void Vfs::touch_meta_(const std::string& p, bool is_new){
  auto &m = meta_[p];
  if (is_new && m.ctime==0) m.ctime = now_epoch_();
  m.mtime = now_epoch_();
  if (m.mode==0) m.mode = 0o100644;
}

bool Vfs::SetMode(std::string_view path, uint32_t mode){
  std::string norm = NormalizePath(path);
  if (!Exists(norm)) return false;
  meta_[norm].mode = mode;
  touch_meta_(norm, false);
  touch_meta_(NormalizePath(path), is_new);
  if (is_new){ auto &m = meta_[NormalizePath(path)]; if (m.mode==0) m.mode = (def_file_mode_ & ~umask_); }
  touch_meta_(NormalizePath(path), is_new);
  if (is_new){ auto &m = meta_[NormalizePath(path)]; if (m.mode==0) m.mode = (def_file_mode_ & ~umask_); }
  return true;
}
bool Vfs::Utime(std::string_view path, uint64_t epoch){
  std::string norm = NormalizePath(path);
  if (!Exists(norm)) return false;
  auto &m = meta_[norm]; if (m.ctime==0) m.ctime = epoch; m.mtime = epoch;
  return true;
}
bool Vfs::GetMeta(std::string_view path, Meta& out) const {
  std::string norm = NormalizePath(path);
  auto it = meta_.find(norm);
  if (it==meta_.end()) return false;
  out = it->second; return true;
}

bool Vfs::Append(std::string_view path, std::string_view content){
  std::string normA = NormalizePath(path);
  long long delta = (long long)content.size();
  if (!quota_reserve_(normA, delta)) return false;
  std::string normA = NormalizePath(path);
  if (is_immutable_(normA) || locked_(normA)) return false;
  std::string norm = NormalizePath(path);
  auto old = Read(norm);
  std::string data = old.value_or(std::string());
  data.append(content.data(), content.size());
  bool ok = Write(norm, data);
  if (ok) touch_meta_(norm, !old.has_value());
  return ok;
}

bool Vfs::Truncate(std::string_view path, std::size_t new_size){
  std::string normT = NormalizePath(path);
  size_t old = 0; if (Exists(normT)) { auto s = Read(normT); if (s) old = s->size(); }
  long long delta = (long long)new_size - (long long)old;
  if (!quota_reserve_(normT, delta)) return false;
  std::string normT = NormalizePath(path);
  if (is_immutable_(normT) || locked_(normT)) return false;
  std::string norm = NormalizePath(path);
  auto old = Read(norm);
  if (!old) return false;
  std::string data = *old;
  if (new_size < data.size()) data.resize(new_size);
  bool ok = Write(norm, data);
  if (ok) touch_meta_(norm, false);
  return ok;
}

std::optional<std::string> Vfs::ReadRange(std::string_view path, std::size_t offset, std::size_t len) const {
  auto s = Read(path); if (!s) return std::nullopt;
  if (offset > s->size()) return std::string();
  std::size_t n = std::min(len, s->size() - offset);
  return s->substr(offset, n);
}
namespace neon {

bool Vfs::Stat(std::string_view path, bool& is_dir, std::size_t& size) const {
  std::string p = NormalizePath(path);
#if NEON_THREAD_SAFE
  std::lock_guard<std::mutex> _lk(mu_);
#endif
  const Node* n = Traverse(p);
  if (!n) return false;
  is_dir = n->is_dir;
  size = n->is_dir ? 0 : n->content.size();
  return true;
}

static std::unique_ptr<Vfs::Node> clone_node(const Vfs::Node* n){
  if (!n) return nullptr;
  auto out = std::make_unique<Vfs::Node>();
  out->is_dir = n->is_dir;
  out->content = n->content;
  for (auto const& kv : n->children){
    out->children[kv.first] = clone_node(kv.second.get());
  }
  return out;
}

bool Vfs::Copy(std::string_view from, std::string_view to, bool recursive){
  std::string sf = NormalizePath(from);
  std::string dt = NormalizePath(to);
#if NEON_THREAD_SAFE
  std::lock_guard<std::mutex> _lk(mu_);
#endif
  if (sealed_) return false;
  if (sf=="/" || dt=="/") return false;
  // locate source
  Node* s = Traverse(sf);
  if (!s) return false;
  if (s->is_dir && !recursive) return false;
  // dest parent
  auto d_last = dt.find_last_of('/');
  std::string d_parent = (d_last==0)? "/" : std::string(dt.substr(0,d_last));
  std::string d_base = std::string(dt.substr(d_last+1));
  Node* dp = EnsureDir(d_parent);
  if (!dp || !dp->is_dir) return false;
  if (dp->children.count(d_base)) return false; // don't overwrite
  dp->children[d_base] = clone_node(s);
  return true;
}


void Vfs::Walk(const std::function<void(const std::string&, bool, std::size_t)>& cb) const {
  struct Item { std::string path; const Node* n; };
  if (!root_) return;
  std::vector<Item> st; st.push_back({"/", root_.get()});
  while (!st.empty()){
    auto it = st.back(); st.pop_back();
    std::size_t sz = it.n->is_dir ? 0 : it.n->content.size();
    cb(it.path, it.n->is_dir, sz);
    if (it.n->is_dir){
      for (auto const& kv : it.n->children){
        std::string p = it.path == "/" ? ("/"+kv.first) : (it.path + "/" + kv.first);
        st.push_back({p, kv.second.get()});
      }
    }
  }
}

std::string Vfs::FsckJson() const {
  size_t files=0, dirs=0; size_t used=0; bool ok=true;
  std::vector<std::string> issues;
  Walk([&](const std::string& path, bool is_dir, std::size_t sz){
    if (is_dir) ++dirs; else { ++files; used += sz; }
    if (path.find("//") != std::string::npos) { ok=false; issues.push_back("double-slash:"+path); }
    if (path.size()>1 && path.back()=='/') { ok=false; issues.push_back("trailing-slash:"+path); }
  });
  std::ostringstream o;
  o << "{";
  o << "\"ok\":" << (ok ? "true":"false") << ",";
  o << "\"files\":" << files << ",";
  o << "\"dirs\":" << dirs << ",";
  o << "\"used_bytes\":" << used << ",";
  o << "\"quota\":" << quota_bytes_;
  o << ",\"sealed\":" << (sealed_ ? "true":"false");
  o << ",\"issues\":[";
  for (size_t i=0;i<issues.size();++i){ if (i) o << ","; o << "\"" << issues[i] << "\""; }
  o << "]";
  o << "}";
  return o.str();
}


bool Vfs::Remove(std::string_view path, bool recursive){
  std::string normR = NormalizePath(path);
  size_t old = 0; if (Exists(normR)) { auto s = Read(normR); if (s) old = s->size(); }

  std::string normR = NormalizePath(path);
  if (is_immutable_(normR) || locked_(normR)) return false;
  std::string normR = NormalizePath(path);
  if (is_immutable_(normR)) return false;
  std::string norm = NormalizePath(path);
#if NEON_THREAD_SAFE
  std::lock_guard<std::mutex> _lk(mu_);
#endif
  if (sealed_) return false;
  if (norm=="/") return false;
  // Find parent and key
  auto last = norm.find_last_of('/');
  std::string parent = (last==0)? "/" : std::string(norm.substr(0,last));
  std::string base = std::string(norm.substr(last+1));
  Node* p = Traverse(parent);
  if (!p || !p->is_dir) return false;
  auto it = p->children.find(base);
  if (it==p->children.end()) return false;
  if (it->second->is_dir && !it->second->children.empty() && !recursive) return false;
  p->children.erase(it);
  return true;
}

bool Vfs::Move(std::string_view from, std::string_view to){
  std::string sf = NormalizePath(from);
  std::string dt = NormalizePath(to);
#if NEON_THREAD_SAFE
  std::lock_guard<std::mutex> _lk(mu_);
#endif
  if (sealed_) return false;
  if (sf=="/" || dt=="/") return false;
  auto s_last = sf.find_last_of('/');
  std::string s_parent = (s_last==0)? "/" : std::string(sf.substr(0,s_last));
  std::string s_base = std::string(sf.substr(s_last+1));
  Node* sp = Traverse(s_parent);
  if (!sp || !sp->is_dir) return false;
  auto s_it = sp->children.find(s_base);
  if (s_it==sp->children.end()) return false;
  // destination parent
  auto d_last = dt.find_last_of('/');
  std::string d_parent = (d_last==0)? "/" : std::string(dt.substr(0,d_last));
  std::string d_base = std::string(dt.substr(d_last+1));
  Node* dp = EnsureDir(d_parent);
  if (!dp || !dp->is_dir) return false;
  if (dp->children.count(d_base)) return false; // don't overwrite
  dp->children[d_base] = std::move(s_it->second);
  sp->children.erase(s_it);
  return true;
}


bool Vfs::BeginTx(){
  if (tx_active_) return false;
  std::string s;
  if (!SaveDumpToString(s)) return false;
  tx_snapshot_ = std::move(s);
  tx_active_ = true;
  return true;
}
bool Vfs::CommitTx(){
  if (!tx_active_) return false;
  tx_active_ = false;
  tx_snapshot_.clear();
  return true;
}
bool Vfs::RollbackTx(){
  if (!tx_active_) return false;
  bool ok = LoadDumpFromString(tx_snapshot_);
  tx_active_ = false;
  tx_snapshot_.clear();
  return ok;
}
bool Vfs::TxActive() const { return tx_active_; }

bool Vfs::SaveDumpToString(std::string& out) const {
  char fname[] = "/tmp/neon_dump_XXXXXX";
#ifdef _WIN32
  // Very simple temp path fallback on Windows
  char tmpPath[MAX_PATH]; GetTempPathA(MAX_PATH, tmpPath);
  std::string t = std::string(tmpPath) + "neon_dump.txt";
  std::string fn = t;
  if (!SaveDump(fn.c_str())) return false;
  std::ifstream in(fn, std::ios::binary);
  if (!in.is_open()) return false;
  out.assign((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  in.close();
  std::remove(fn.c_str());
  return true;
#else
  int fd = mkstemp(fname);
  if (fd < 0) return false;
  close(fd);
  std::string fn = fname;
  bool ok = SaveDump(fn.c_str());
  if (!ok) { std::remove(fn.c_str()); return false; }
  std::ifstream in(fn, std::ios::binary);
  if (!in.is_open()) { std::remove(fn.c_str()); return false; }
  out.assign((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  in.close();
  std::remove(fn.c_str());
  return true;
#endif
}

bool Vfs::LoadDumpFromString(const std::string& in){
  char fname[] = "/tmp/neon_dump_in_XXXXXX";
#ifdef _WIN32
  char tmpPath[MAX_PATH]; GetTempPathA(MAX_PATH, tmpPath);
  std::string fn = std::string(tmpPath) + "neon_dump_in.txt";
  {
    std::ofstream o(fn, std::ios::binary); if (!o.is_open()) return false;
    o.write(in.data(), (std::streamsize)in.size());
  }
  bool ok = LoadDump(fn.c_str());
  std::remove(fn.c_str());
  return ok;
#else
  int fd = mkstemp(fname);
  if (fd < 0) return false;
  std::string fn = fname;
  {
    FILE* f = fdopen(fd, "wb");
    if (!f) { close(fd); return false; }
    fwrite(in.data(), 1, in.size(), f);
    fclose(f);
  }
  bool ok = LoadDump(fn.c_str());
  std::remove(fn.c_str());
  return ok;
#endif
}


void Vfs::SetQuota(std::size_t bytes){ quota_bytes_ = bytes; }
std::size_t Vfs::Quota() const { return quota_bytes_; }

std::size_t Vfs::calc_used_(const Node* n) const {
  if (!n) return 0;
  std::size_t total = 0;
  if (!n->is_dir) total += n->content.size();
  for (const auto& kv : n->children) total += calc_used_(kv.second.get());
  return total;
}
std::size_t Vfs::UsedBytes() const {
  return root_ ? calc_used_(root_.get()) : 0;
}
namespace neon {

void Vfs::Seal(bool on){ sealed_ = on; }
bool Vfs::Sealed() const { return sealed_; }
namespace neon {

std::string Vfs::NormalizePath(std::string_view in) {
  std::vector<std::string> parts;
  std::string cur;
  auto flush=[&]{ if(!cur.empty()){ parts.push_back(cur); cur.clear(); } };
  for(char c: in){
    if (c=='/') flush();
    else cur.push_back(c);
  }
  flush();
  std::vector<std::string> stack;
  for (auto& p: parts){
    if (p=="" || p==".") continue;
    if (p=="..") { if(!stack.empty()) stack.pop_back(); continue; }
    stack.push_back(p);
  }
  std::string out = "/";
  for (size_t i=0;i<stack.size();++i){
    out += stack[i];
    if (i+1<stack.size()) out += "/";
  }
  return out;
}


Vfs::Vfs() : root_(std::make_unique<Node>()) {
  root_->is_dir = true;
  root_->name = "/";
}

neon::Node* Vfs::Traverse(std::string_view path) const {
  std::string norm = NormalizePath(path);
  path = norm;
  if (path.empty() || path == "/") return root_.get();
  Node* cur = root_.get();
  std::string seg;
  for (char c : path) {
    if (c == '/') {
      if (!seg.empty()) {
        auto it = cur->children.find(seg);
        if (it == cur->children.end()) return nullptr;
        cur = it->second.get();
        seg.clear();
      }
    } else {
      seg.push_back(c);
    }
  }
  if (!seg.empty()) {
    auto it = cur->children.find(seg);
    if (it == cur->children.end()) return nullptr;
    cur = it->second.get();
  }
  return cur;
}

neon::Node* Vfs::EnsureDir(std::string_view path) {
  std::string norm = NormalizePath(path);
  path = norm;
  if (path.empty() || path == "/") return root_.get();
  Node* cur = root_.get();
  std::string seg;
  for (char c : path) {
    if (c == '/') {
      if (!seg.empty()) {
        auto& child = cur->children[seg];
        if (!child) {
          child = std::make_unique<Node>();
          child->is_dir = true;
          child->name = seg;
        }
        if (!child->is_dir) return nullptr;
        cur = child.get();
        seg.clear();
      }
    } else {
      seg.push_back(c);
    }
  }
  if (!seg.empty()) {
    auto& child = cur->children[seg];
    if (!child) {
      child = std::make_unique<Node>();
      child->is_dir = true;
      child->name = seg;
    }
    if (!child->is_dir) return nullptr;
    cur = child.get();
  }
  return cur;
}

bool Vfs::Mkdir(std::string_view path) {
  bool is_new = !Exists(std::string(path));
#if NEON_THREAD_SAFE
  std::lock_guard<std::mutex> _lk(mu_);
#endif
 if (sealed_) return false;
  std::string norm = NormalizePath(path);
  path = norm;
  return EnsureDir(path) != nullptr;
}

bool Vfs::Touch(std::string_view path) {
#if NEON_THREAD_SAFE
  std::lock_guard<std::mutex> _lk(mu_);
#endif
 if (sealed_) return false;
  std::string norm = NormalizePath(path);
  path = norm;
  if (path.empty() || path == "/") return false;
  std::string p(path);
  auto pos = p.find_last_of('/');
  std::string dir = pos == std::string::npos ? "/" : p.substr(0, pos);
  std::string name = pos == std::string::npos ? p : p.substr(pos+1);
  if (name.empty()) return false;
  Node* d = EnsureDir(dir);
  if (!d || !d->is_dir) return false;
  if (!d->children.count(name)) {
    auto f = std::make_unique<Node>();
    f->is_dir = false;
    f->name = name;
    d->children.emplace(name, std::move(f));
  }
  return true;
}

bool Vfs::Write(std::string_view path, std::string_view content){
  bool is_new = !Exists(std::string(path));
  /* QUOTA */
  if (quota_bytes_>0) {
    std::size_t old = 0; if (auto p = Read(std::string(path)); p.has_value()) old = p->size();
    std::size_t used = UsedBytes();
    if (used - old + content.size() > quota_bytes_) return false;
  }
#if NEON_THREAD_SAFE
  std::lock_guard<std::mutex> _lk(mu_);
#endif
 if (sealed_) return false;
  std::string norm = NormalizePath(path);
  path = norm;
  Node* n = Traverse(path);
  if (!n) {
    // auto-create file in parent if path exists
    std::string p(path);
    auto pos = p.find_last_of('/');
    std::string dir = pos == std::string::npos ? "/" : p.substr(0, pos);
    std::string name = pos == std::string::npos ? p : p.substr(pos+1);
    Node* d = EnsureDir(dir);
    if (!d || !d->is_dir || name.empty()) return false;
    auto f = std::make_unique<Node>();
    f->is_dir = false;
    f->name = name;
    n = (d->children[name] = std::move(f)).get();
  }
  if (n->is_dir) return false;
  n->content.assign(content.begin(), content.end());
  return true;
}

std::optional<std::string> Vfs::Read(std::string_view path) const {
  std::string norm = NormalizePath(path);
  path = norm;
  Node* n = Traverse(path);
  if (!n || n->is_dir) return std::nullopt;
  return n->content;
}

std::vector<std::string> Vfs::List(std::string_view path) const {
  std::string norm = NormalizePath(path);
  path = norm;
  std::vector<std::string> out;
  Node* n = Traverse(path);
  if (!n || !n->is_dir) std::sort(out.begin(), out.end());
  return out;
  for (auto& kv : n->children) out.push_back(kv.first + (kv.second->is_dir ? "/" : ""));
  return out;
}

bool Vfs::Exists(std::string_view path) const {
  std::string norm = NormalizePath(path);
  path = norm;
  return Traverse(path) != nullptr;
}

}  // namespace neon
