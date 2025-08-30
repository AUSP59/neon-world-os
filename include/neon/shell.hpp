// SPDX-License-Identifier: MIT
#pragma once
#include <unordered_map>
#include <set>
#include "neon/export.hpp"
#include <string>
#include <vector>
#include "neon/scheduler.hpp"
#include "neon/vfs.hpp"

namespace neon {

struct AclEntry{ std::string prefix; uint32_t perms=0; }; // perms bits: 1=R,2=W,4=X,8=A(admin)
  std::unordered_map<std::string, std::vector<AclEntry>> acl_roles_;
  bool acl_enforce_ = false; std::string current_role_ = "root";
  bool has_perm_(const std::string& role, const std::string& path, uint32_t need) const;


class NEON_API Shell {
 public:
  Shell(Scheduler& sched, Vfs& vfs);
  std::string Exec(const std::string& line);

 
private:
  bool dlp_on_ = false;
  bool wal_on_ = false;
  std::unordered_map<std::string, long long> metrics_count_;
  std::unordered_map<std::string, long long> metrics_total_us_;
  bool metrics_on_ = false;
  std::unordered_map<std::string, std::string> vars_;
  std::vector<std::string> redacts_;
  std::string expand_vars_(const std::string& in) const;
  bool retain_on_ = true;

struct PolicyRule{
  bool allow;
  std::string cmd_re;
  std::string path_re;
  std::string reason;
};
  std::vector<PolicyRule> policies_;
  bool safemode_on_ = false;
  bool index_auto_on_ = false;
  bool trace_on_ = false; std::vector<std::string> trace_events_;
  bool prov_on_ = true;
  void index_remove_path_(const std::string& path);
  void index_add_path_(const std::string& path, const std::string& content);

struct RateLimit{ double capacity=0, tokens=0, refill_per_s=0; long long last=0; };
  std::unordered_map<std::string, RateLimit> rl_;
  std::unordered_map<std::string, std::string> cfg_schema_;
  bool versions_on_ = true;
  bool txn_on_ = false; std::string txn_dump_;
  std::string ns_current_ = "";
  bool autosnap_on_ = false; size_t autosnap_keep_ = 10; size_t autosnap_seq_ = 0; std::vector<std::string> autosnaps_;
  bool hook_exec_ = false; struct Hook{ std::string evt; std::string prefix; std::string cmd; }; std::vector<Hook> hooks_;
  std::unordered_map<std::string,std::string> snap_sig_;
  uint32_t perm_for_cmd_(const std::vector<std::string>& toks, std::set<std::string>& paths) const;
  std::unordered_map<std::string, std::unordered_set<std::string>> index_words_;
  size_t index_docs_ = 0; size_t index_terms_ = 0;
  bool undo_on_ = true; size_t undo_cap_ = 16; std::deque<std::string> undo_stack_;
  std::unordered_map<std::string,std::vector<std::string>> macros_;
  bool macro_rec_ = false; std::string macro_name_;
  std::unordered_map<std::string,std::string> vars_;
  std::unordered_map<std::string,std::string> snap_desc_;
  std::unordered_map<std::string, std::vector<long long>> durations_cmd_;
  std::string audit_key_;
  std::string audit_prev_tag_;
  std::unordered_map<std::string,std::string> aliases_;
  bool journal_on_ = false; std::string journal_path_;
  uint32_t umask_current_ = 0022;
  // Metrics & rate limiting
  long long metrics_total_ = 0;
  long long metrics_errors_ = 0;
  std::unordered_map<std::string, long long> metrics_cmd_;
  std::unordered_map<std::string, int> rate_per_min_;
  std::unordered_map<std::string, std::pair<long long,int>> rate_bucket_;
  bool dryrun_ = false;
  std::vector<std::regex> audit_redact_;
  std::unordered_map<std::string, std::string> snapshots_;
  bool audit_json_ = false; std::size_t audit_rotate_bytes_ = 0;
  long long limit_time_us_ = 0; // 0 = disabled
  bool policy_on_ = false;
  std::unordered_set<std::string> allow_;
  bool audit_on_ = false;
  std::string audit_path_;
  std::vector<std::string> history_;
  Scheduler& sched_;
  Vfs& vfs_;
  std::vector<std::string> Tokenize(const std::string& line) const;
};

}  // namespace neon
