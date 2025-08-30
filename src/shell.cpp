#include "neon/telemetry.hpp"
// SPDX-License-Identifier: MIT

#include "neon/shell.hpp"
#include "neon/util.hpp"
#include "neon/plugin.hpp"
#include <unordered_set>
#include <thread>
#include "neon/trace.hpp"
#include "neon/schemas.hpp"
#include "neon/crypto.hpp"
#include "neon/merkle.hpp"
#include "neon/base64.hpp"
#include "neon/vfs.hpp"
#include <sstream>
#include <algorithm>
#include <set>
#include <map>
#include <ctime>
#include <iomanip>
#include <random>
#include <fstream>
#include <filesystem>
#include <regex>
#include <chrono>







namespace neon {

std::string Shell::expand_vars_(const std::string& in) const {
  std::string out; out.reserve(in.size());
  for (size_t i=0;i<in.size();){
    if (in[i]=='$' && i+1<in.size() && in[i+1]=='{'){
      size_t j = in.find('}', i+2);
      if (j!=std::string::npos){
        std::string key = in.substr(i+2, j-(i+2));
        auto it = vars_.find(key);
        if (it!=vars_.end()) out += it->second;
        i = j+1;
        continue;
      }
    }
    out.push_back(in[i++]);
  }
  return out;
}
namespace neon {

static int levenshtein_(const std::string& a, const std::string& b){
  size_t n=a.size(), m=b.size();
  std::vector<int> dp(m+1);
  for (size_t j=0;j<=m;++j) dp[j]=(int)j;
  for (size_t i=1;i<=n;++i){
    int prev=dp[0]; dp[0]=(int)i;
    for (size_t j=1;j<=m;++j){
      int tmp=dp[j];
      if (a[i-1]==b[j-1]) dp[j]=prev;
      else dp[j]=std::min({prev+1, dp[j]+1, dp[j-1]+1});
      prev=tmp;
    }
  }
  return dp[m];
}
static long long parse_size_(const std::string& s){
  // supports: B, K, KB, KiB, M, MB, MiB, G, GB, GiB
  std::string t; for (char c: s) if (c!=' ') t.push_back((char)std::toupper((unsigned char)c));
  long double num=0; int i=0; bool dot=false; long double frac=0, base=1; long double div=1;
  while (i<(int)t.size() && (isdigit((unsigned char)t[i])||t[i]=='.')){ if (t[i]=='.'){ dot=true; ++i; continue; } if (!dot){ num = num*10 + (t[i]-'0'); } else { frac = frac*10 + (t[i]-'0'); div*=10; } ++i; }
  long double val = num + (div>1? frac/div : 0);
  std::string u = t.substr(i);
  if (u=="B" || u=="") base=1;
  else if (u=="K" || u=="KB") base=1000;
  else if (u=="KI" || u=="KIB") base=1024;
  else if (u=="M" || u=="MB") base=1000*1000;
  else if (u=="MI" || u=="MIB") base=1024*1024;
  else if (u=="G" || u=="GB") base=1000*1000*1000;
  else if (u=="GI" || u=="GIB") base=1024.0*1024.0*1024.0;
  else base=1;
  long double out = val*base; if (out<0) out=0; if (out>9.22e18) out=9.22e18;
  return (long long)(out+0.5);
}
static long long parse_duration_s_(const std::string& s){
  // supports: s, m, h, d
  std::string t; for (char c: s) if (c!=' ') t.push_back((char)std::tolower((unsigned char)c));
  long long num=0; int i=0; while (i<(int)t.size() && isdigit((unsigned char)t[i])){ num = num*10 + (t[i]-'0'); ++i; }
  std::string u = t.substr(i);
  if (u=="h") num*=3600;
  else if (u=="m") num*=60;
  else if (u=="d") num*=86400;
  else /*s or empty*/;
  return num;
}
namespace neon {

void Shell::index_remove_path_(const std::string& path){
  for (auto &kv : index_words_){
    kv.second.erase(path);
  }
}
void Shell::index_add_path_(const std::string& path, const std::string& content){
  // naive tokenize: a-z0-9 words, lowercased
  auto lower=[&](char c)->char{ return (char)std::tolower((unsigned char)c); };
  std::string w;
  auto flush=[&](){ if (!w.empty()){ index_words_[w].insert(path); w.clear(); } };
  for (char c : content){
    if (std::isalnum((unsigned char)c)) w.push_back(lower(c));
    else flush();
  }
  flush();
}
namespace neon {
/* NS_HELPER */
static std::string ns_apply_(const std::string& ns, const std::string& path){
  if (ns.empty()) return path;
  if (path.empty() || path[0] != '/') return path;
  if (path.rfind("/@ns/", 0) == 0) return path; // already namespaced
  if (path == "/") return std::string("/@ns/") + ns;
  return std::string("/@ns/") + ns + path;
}
namespace neon {

bool Shell::has_perm_(const std::string& role, const std::string& path, uint32_t need) const{
  if (role=="root") return true;
  auto it = acl_roles_.find(role);
  if (it==acl_roles_.end()) return false;
  uint32_t acc = 0;
  for (auto const& e : it->second){
    if (path.rfind(e.prefix, 0) == 0){ acc |= e.perms; }
  }
  return (acc & need) == need;
}

uint32_t Shell::perm_for_cmd_(const std::vector<std::string>& toks, std::set<std::string>& paths) const{
  if (toks.empty()) return 0;
  auto add=[&](const std::string& p){ paths.insert(p); };
  std::string c = toks[0];
  auto needR = 1u, needW = 2u, needA = 8u;
  if (c=="echo"||c=="append"||c=="truncate"||c=="rm"||c=="mkdir"||c=="chmod"||c=="utime"||c=="xattr"||c=="repl"||c=="chattr"){
    if (toks.size()>=3){ add(toks.back()); return needW; }
    return needW;
  }
  if (c=="mv"){ if (toks.size()>=3){ add(toks[1]); add(toks[2]); } return needW; }
  if (c=="cp"){ if (toks.size()>=3){ add(toks[1]); add(toks[2]); } return needR|needW; }
  if (c=="cat"||c=="catrange"||c=="stat"||c=="ls"||c=="grep"||c=="wc"||c=="hexdump"||c=="sha256"||c=="verifyseal"||c=="merkleroot"){
    if (toks.size()>=2){ add(toks.back()); } return needR;
  }
  if (c=="seal"){ if (toks.size()>=2){ add(toks[1]); } return needW; }
  if (c=="lock"){ if (toks.size()>=3){ add(toks[2]); } return needA; }
  if (c=="index"){ return needR; }
  if (c=="exportfs"||c=="importfs"||c=="importdelta"||c=="fsck"||c=="verify"||c=="config"||c=="undo"||c=="rate"||c=="audit"||c=="journal"||c=="umask"||c=="policy"||c=="snap"){
    return needA;
  }
  return 0;
}
namespace neon {

static bool glob_match(const std::string& s, const std::string& p){
  size_t i=0,j=0,star=std::string::npos,mark=0;
  while (i<s.size()){
    if (j<p.size() && (p[j]=='?' || p[j]==s[i])){ ++i; ++j; }
    else if (j<p.size() && p[j]=='*'){ star=j++; mark=i; }
    else if (star!=std::string::npos){ j=star+1; i=++mark; }
    else return false;
  }
  while (j<p.size() && p[j]=='*') ++j;
  return j==p.size();
}


void Shell::Register(const std::string& name, CommandFn fn){ registry_[name] = std::move(fn); }


static std::string JsonEscape(const std::string& s){
  std::ostringstream o;
  o << '"';
  for (char c : s) {
    switch (c) {
      case '"': o << "\\\""; break;
      case '\\': o << "\\\\"; break;
      case '\b': o << "\\b"; break;
      case '\f': o << "\\f"; break;
      case '\n': o << "\\n"; break;
      case '\r': o << "\\r"; break;
      case '\t': o << "\\t"; break;
      default:
        if ((unsigned char)c < 0x20) { o << "\\u" << std::hex << (int)c; }
        else { o << c; }
    }
  }
  o << '"';
  
// long format when "-l"
if (a.size()>=2 && a[1]=="-l"){
  std::ostringstream l;
  for (auto& e: out){
    std::string p = (toks.size()>1? toks[1] : "/");
    if (p=="-l") p = "/";
    std::string full = p=="/" ? ("/"+e) : (p + "/" + e);
    bool is_dir=false; std::size_t size=0; vfs_.Stat(full, is_dir, size);
    neon::Vfs::Meta m{}; vfs_.GetMeta(full, m);
    l << (is_dir? 'd':'-') << std::oct << m.mode << std::dec << " " << size << " " << m.mtime << " " << e << "\\n";
  }
  return l.str();
}
return o.str();
}

static std::string JsonArray(const std::vector<std::string>& items){
  std::ostringstream o; o << '[';
  for (size_t i=0;i<items.size();++i){
    if (i) o << ',';
    o << JsonEscape(items[i]);
  }
  o << ']'; 
// long format when "-l"
if (a.size()>=2 && a[1]=="-l"){
  std::ostringstream l;
  for (auto& e: out){
    std::string p = (toks.size()>1? toks[1] : "/");
    if (p=="-l") p = "/";
    std::string full = p=="/" ? ("/"+e) : (p + "/" + e);
    bool is_dir=false; std::size_t size=0; vfs_.Stat(full, is_dir, size);
    neon::Vfs::Meta m{}; vfs_.GetMeta(full, m);
    l << (is_dir? 'd':'-') << std::oct << m.mode << std::dec << " " << size << " " << m.mtime << " " << e << "\\n";
  }
  return l.str();
}
return o.str();
}

Shell::Shell(Scheduler& sched, Vfs& vfs) : sched_(sched), vfs_(vfs) {
  registry_["help"] = [&](const std::vector<std::string>&){ return "Commands: ls, cat, echo, mkdir, touch, ps, kill, meminfo, help, mode, savefs, loadfs, tree, plugin"; };
  registry_["mode"] = [&](const std::vector<std::string>& a){
    if (a.size() > 1 && a[1] == "json") { json_mode_ = true; return "ok"; }
    if (a.size() > 1 && a[1] == "text") { json_mode_ = false; return "ok"; }
    return json_mode_ ? "json" : "text";
  };
  // enhanced ls
registry_["ls"] = [&](const std::vector<std::string>& a){
    std::string path = a.size()>1 ? a[1] : "/";
    auto items = vfs_.List(path);
    if (json_mode_) return JsonArray(items);
    std::ostringstream oss; for (auto& i:items) oss<<i<<"\n"; return oss.str();
  };
  registry_["mkdir"] = [&](const std::vector<std::string>& a){
    return (a.size()>1 && vfs_.Mkdir(a[1])) ? "ok" : "err";
  };
  registry_["touch"] = [&](const std::vector<std::string>& a){
    return (a.size()>1 && vfs_.Touch(a[1])) ? "ok" : "err";
  };
  registry_["echo"] = [&](const std::vector<std::string>& a){
    if (a.size()<=2) return std::string("err");
    return vfs_.Write(a[2], a[1]) ? "ok" : "err";
  };
  registry_["cat"] = [&](const std::vector<std::string>& a){
    if (a.size()<=1) return std::string("err");
    auto d = vfs_.Read(a[1]);
    if (!d) return std::string("err");
    if (json_mode_) { return std::string("{\"path\":")+JsonEscape(a[1])+std::string(",\"data\":")+JsonEscape(*d)+"}"; }
    return *d;
  };
  registry_["ps"] = [&](const std::vector<std::string>&){
    auto snap = sched_.Snapshot();
    if (json_mode_) {
      std::ostringstream o; o << '[';
      for (size_t i=0;i<snap.size();++i){
        if (i) o << ',';
        o << "{\"id\":" << snap[i].id << ",\"name\":" << JsonEscape(snap[i].name) << "}";
      }
      o << ']'; 
// long format when "-l"
if (a.size()>=2 && a[1]=="-l"){
  std::ostringstream l;
  for (auto& e: out){
    std::string p = (toks.size()>1? toks[1] : "/");
    if (p=="-l") p = "/";
    std::string full = p=="/" ? ("/"+e) : (p + "/" + e);
    bool is_dir=false; std::size_t size=0; vfs_.Stat(full, is_dir, size);
    neon::Vfs::Meta m{}; vfs_.GetMeta(full, m);
    l << (is_dir? 'd':'-') << std::oct << m.mode << std::dec << " " << size << " " << m.mtime << " " << e << "\\n";
  }
  return l.str();
}
return o.str();
    }
    std::ostringstream oss; for (auto& t:snap) oss<<t.id<<" "<<t.name<<"\n"; return oss.str();
  };
  registry_["kill"] = [&](const std::vector<std::string>&){
    sched_.Stop(); return "stopped";
  };
  
registry_["savefs"] = [&](const std::vector<std::string>& a){ /* sha256 of file supported with --sha256 */
  if (a.size()<=1) return std::string("err");
  
bool ok = vfs_.SaveDump(a[1]);
if (!ok) return std::string("err");
if (a.size()>=3 && a[2]=="--sha256"){
  std::ifstream in(a[1], std::ios::binary);
  if (!in.is_open()) return std::string("err");
  neon::Sha256 h; char buf[4096];
  while (in.good()){
    in.read(buf, sizeof(buf));
    auto n = in.gcount();
    if (n>0) h.update(buf, (size_t)n);
  }
  std::ostringstream o; o << "ok " << neon::Sha256::hex(h.digest());
  
// long format when "-l"
if (a.size()>=2 && a[1]=="-l"){
  std::ostringstream l;
  for (auto& e: out){
    std::string p = (toks.size()>1? toks[1] : "/");
    if (p=="-l") p = "/";
    std::string full = p=="/" ? ("/"+e) : (p + "/" + e);
    bool is_dir=false; std::size_t size=0; vfs_.Stat(full, is_dir, size);
    neon::Vfs::Meta m{}; vfs_.GetMeta(full, m);
    l << (is_dir? 'd':'-') << std::oct << m.mode << std::dec << " " << size << " " << m.mtime << " " << e << "\\n";
  }
  return l.str();
}
return o.str();
}
return std::string("ok");

};
registry_["loadfs"] = [&](const std::vector<std::string>& a){
  if (a.size()<=1) return std::string("err");
  return vfs_.LoadDump(a[1]) ? std::string("ok") : std::string("err");
};
registry_["tree"] = [&](const std::vector<std::string>& a){
  std::string path = a.size()>1 ? a[1] : "/";
  auto items = vfs_.List(path);
  // simple tree view: just list children with slashes preserved
  std::ostringstream o; for (auto& i:items) o<<i<<"\n"; 
// long format when "-l"
if (a.size()>=2 && a[1]=="-l"){
  std::ostringstream l;
  for (auto& e: out){
    std::string p = (toks.size()>1? toks[1] : "/");
    if (p=="-l") p = "/";
    std::string full = p=="/" ? ("/"+e) : (p + "/" + e);
    bool is_dir=false; std::size_t size=0; vfs_.Stat(full, is_dir, size);
    neon::Vfs::Meta m{}; vfs_.GetMeta(full, m);
    l << (is_dir? 'd':'-') << std::oct << m.mode << std::dec << " " << size << " " << m.mtime << " " << e << "\\n";
  }
  return l.str();
}
return o.str();
};
  
registry_["history"] = [&](const std::vector<std::string>& a){
  size_t n = a.size()>1 ? static_cast<size_t>(std::stoul(a[1])) : history_.size();
  std::ostringstream o; size_t start = history_.size()>n ? history_.size()-n : 0;
  for (size_t i=start;i<history_.size();++i) o << history_[i] << "\n";
  
// long format when "-l"
if (a.size()>=2 && a[1]=="-l"){
  std::ostringstream l;
  for (auto& e: out){
    std::string p = (toks.size()>1? toks[1] : "/");
    if (p=="-l") p = "/";
    std::string full = p=="/" ? ("/"+e) : (p + "/" + e);
    bool is_dir=false; std::size_t size=0; vfs_.Stat(full, is_dir, size);
    neon::Vfs::Meta m{}; vfs_.GetMeta(full, m);
    l << (is_dir? 'd':'-') << std::oct << m.mode << std::dec << " " << size << " " << m.mtime << " " << e << "\\n";
  }
  return l.str();
}
return o.str();
};

registry_["plugin"] = [&](const std::vector<std::string>& a){
  if (a.size()>=3 && a[1]=="load") {
    return LoadPlugin(*this, a[2]);
  }
  return std::string("err");
};

registry_["audit"] = [&](const std::vector<std::string>& a){
  if (a.size()>=2 && a[1]=="on"){
    if (a.size()>=3) audit_path_ = a[2];
    if (audit_path_.empty()) audit_path_ = "neon_audit.log";
    audit_on_ = true; return std::string("ok");
  }
  if (a.size()>=2 && a[1]=="off"){ audit_on_ = false; return std::string("ok"); }
  
if (a.size()>=3 && a[1]=="key"){ audit_key_ = a[2]; audit_prev_tag_.clear(); return std::string("ok"); }
if (a.size()>=2 && a[1]=="status"){
 return audit_on_ ? (audit_path_.empty() ? "on" : "on:"+audit_path_) : "off"; }
  return std::string("err");
};

registry_["seal"] = [&](const std::vector<std::string>& a){
  if (a.size()>=2 && (a[1]=="on"||a[1]=="off"||a[1]=="status")){
    if (a[1]=="on") { vfs_.Seal(true); return "on"; }
    if (a[1]=="off") { vfs_.Seal(false); return "off"; }
    return vfs_.Sealed() ? "on" : "off";
  }
  return std::string("err");
};
registry_["policy"] = [&](const std::vector<std::string>& a){
  if (a.size()>=2 && a[1]=="on"){ policy_on_ = true; return "on"; }
  if (a.size()>=2 && a[1]=="off"){ policy_on_ = false; return "off"; }
  if (a.size()>=3 && a[1]=="allow"){ allow_.insert(a[2]); return "ok"; }
  if (a.size()>=3 && a[1]=="deny"){ allow_.erase(a[2]); return "ok"; }
  
if (a.size()>=3 && a[1]=="key"){ audit_key_ = a[2]; audit_prev_tag_.clear(); return std::string("ok"); }
if (a.size()>=2 && a[1]=="status"){
 return policy_on_ ? "on" : "off"; }
  return std::string("err");
};

registry_["time"] = [&](const std::vector<std::string>& a){
  if (a.size()<2) return std::string("err");
  std::ostringstream cmd;
  for (size_t i=1;i<a.size();++i){ if (i>1) cmd<<' '; cmd<<a[i]; }
  using namespace std::chrono;
  auto t0 = steady_clock::now();
  auto out = Exec(cmd.str());
  auto t1 = steady_clock::now();
  auto us = duration_cast<microseconds>(t1 - t0).count();
  std::ostringstream o; o << out << "\\n" << "[time " << us << "us]";
  
// long format when "-l"
if (a.size()>=2 && a[1]=="-l"){
  std::ostringstream l;
  for (auto& e: out){
    std::string p = (toks.size()>1? toks[1] : "/");
    if (p=="-l") p = "/";
    std::string full = p=="/" ? ("/"+e) : (p + "/" + e);
    bool is_dir=false; std::size_t size=0; vfs_.Stat(full, is_dir, size);
    neon::Vfs::Meta m{}; vfs_.GetMeta(full, m);
    l << (is_dir? 'd':'-') << std::oct << m.mode << std::dec << " " << size << " " << m.mtime << " " << e << "\\n";
  }
  return l.str();
}
return o.str();
};
registry_["trace"] = [&](const std::vector<std::string>& a){
  if (a.size()>=2 && a[1]=="on"){
    std::string file = a.size()>=3 ? a[2] : "neon_trace.json";
    neon::Trace::Instance().Start(file);
    return std::string("on");
  }
  if (a.size()>=2 && a[1]=="off"){ neon::Trace::Instance().Stop(); return std::string("off"); }
  return std::string("err");
};

registry_["schema"] = [&](const std::vector<std::string>& a){
  if (a.size()<2) return std::string("err");
  if (auto* js = neon::JsonSchema(a[1])) return std::string(js);
  return std::string("err");
};
registry_["meminfo"] = [&](const std::vector<std::string>&){
    return "N/A";
  };
}


std::vector<std::string> Shell::Tokenize(const std::string& line) const {
  std::vector<std::string> out; std::string cur; bool in_s=false, in_d=false, esc=false;
  for (char c : line) {
    if (esc) { cur.push_back(c); esc=false; continue; }
    if (c == '\\') { esc=true; continue; }
    if (c == '\'' && !in_d) { in_s = !in_s; continue; }
    if (c == '\"' && !in_s) { in_d = !in_d; continue; }
    if (c == ' ' && !in_s && !in_d) { if (!cur.empty()) { out.push_back(cur); cur.clear(); } continue; }
    cur.push_back(c);
  }
  if (!cur.empty()) out.push_back(cur);
  return out;
}


std::string Shell::Exec(const std::string& line) {
  TraceScope _ts("shell.exec");
  auto toks = Tokenize(line);
  if (toks.empty()) return "";
  history_.push_back(line);
  auto it = registry_.find(toks[0]);
  if (it == registry_.end()) return "unknown";
  

/* VERSIONING PRE */
std::vector<std::pair<std::string,std::string>> _ver_backup;
if (versions_on_ && is_mut && !dryrun_){
  std::set<std::string> vpaths; perm_for_cmd_(toks, vpaths);
  for (auto const& p : vpaths){ auto s = vfs_.Read(p); if (s) _ver_backup.emplace_back(p, *s); }
}


/* POLICY PRE */
if (!toks.empty()){
  // safemode: block destructive commands
  if (safemode_on_){
    static const std::set<std::string> blocked = {"rm","xrm","srm","trash","ttl"};
    if (blocked.count(toks[0])) return std::string("err_denied");
  }
  // policy rules
  std::set<std::string> pths; uint32_t need = perm_for_cmd_(toks, pths);
  (void)need;
  for (auto &rule : policies_){
    try{
      std::regex rc(rule.cmd_re), rp(rule.path_re);
      bool cmd_ok = std::regex_search(toks[0], rc);
      bool path_ok = pths.empty(); // if no paths, apply only cmd rule
      for (auto &p : pths){ if (std::regex_search(p, rp)) { path_ok = true; break; } }
      if (cmd_ok && path_ok){
        if (!rule.allow){ return std::string("err_denied"); }
        break; // first-match allow
      }
    } catch(...) {}
  }
}

/* TRACE START */
using namespace std::chrono;
auto __trace_start = high_resolution_clock::now();


/* VAR EXPAND PRE */
for (auto &tok : toks){ tok = expand_vars_(tok); }

/* HOLD/PRESERVATION PRE */
if (!toks.empty() && retain_on_){
  static const std::set<std::string> delcmd = {"rm","xrm","srm","trash"};
  if (delcmd.count(toks[0])){
    std::set<std::string> pths; perm_for_cmd_(toks, pths);
    long long now = (long long)std::time(nullptr);
    for (auto &p : pths){
      std::string until;
      if (vfs_.XAttrGet(p, "hold:until", until)){
        long long ts = atoll(until.c_str());
        if (ts > now) return std::string("err_hold");
      }
    }
  }
}

/* IMMUT/LEASE PRE */

/* WAL PRE */
using namespace std::chrono;
auto __metrics_start = high_resolution_clock::now();
if (wal_on_ && is_mut){
  long long ts = (long long)std::time(nullptr);
  std::ostringstream rec; rec << ts << " " << join_tokens_(toks) << "\n";
  vfs_.Mkdir("/.wal");
  std::string prev; if (auto s=vfs_.Read("/.wal/log")) prev=*s;
  prev += rec.str();
  vfs_.Write("/.wal/log", prev);
}


/* DLP PRE */
if (dlp_on_ && !toks.empty()){
  static const std::set<std::string> sensitive={"pack","packsign","bundle","share","export"};
  if (sensitive.count(toks[0])){
    auto s1 = Exec("scan secrets /");
    auto s2 = Exec("scan pii /");
    if (s1 != "[]" || s2 != "[]") return std::string("err_dlp");
  }
}
/* RLIMIT PRE */
if (!toks.empty()){
  auto itrl = rl_.find(toks[0]);
  if (itrl != rl_.end()){
    long long now = (long long)std::time(nullptr);
    auto &r = itrl->second;
    double elapsed = (r.last==0)? 0.0 : double(now - r.last);
    r.last = now;
    r.tokens = std::min(r.capacity, r.tokens + elapsed * r.refill_per_s);
    if (r.tokens < 1.0){ return std::string("err_rate"); }
    r.tokens -= 1.0;
  }
}
auto result = it->second(toks);


// AUDIT
if (audit_on_) {
    /* AUDIT CHAIN */
    bool chained = audit_json_ && !audit_key_.empty();

    /* REDACT */
    auto apply_redact = [&](std::string s){ for (auto const& rx : audit_redact_) { s = std::regex_replace(s, rx, "***"); } return s; };
  std::ofstream out(audit_path_, std::ios::app);
  if (out.is_open()) {
      if (audit_json_ && chained) {
        using namespace std::chrono;
        auto ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        std::ostringstream data; data << audit_prev_tag_ << ts << "|" << line << "|" << result;
        std::string tag = neon::HmacSha256(audit_key_, data.str());
        audit_prev_tag_ = tag;
        out << "{"ts":"" << ts << "","cmd":"" << apply_redact(line) << "","out":"" << apply_redact(result) << "","tag":"" << tag << ""}\n";
        return result;
      }

    using namespace std::chrono;
    auto ts = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    out << ts << " | " << apply_redact(line) << " | " << result << "\n";
  }
}
return result;

}

} // namespace neon


namespace neon {

static void cmd_metrics(Shell& sh, Args&&){
  auto& vfs = sh.vfs();
  auto& mm  = sh.memory();
  uint64_t files=0, dirs=0;
  vfs.walk("/", [&](const std::string& p, bool is_dir){ if (is_dir) ++dirs; else ++files; return true; });
  auto txt = Telemetry::instance().prometheusText(files, dirs, mm.bytes_used(), mm.capacity());
  sh.out() << txt << "\n";
}

static void cmd_trace(Shell& sh, Args&& a){
  if (a.size()==1 && a[0]=="dump"){
    auto json = Telemetry::instance().dumpTraceJson();
    sh.out() << json << "\n";
    return;
  }
  sh.err() << "usage: trace dump\n";
}

} // namespace neon
