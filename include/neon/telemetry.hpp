#pragma once
// SPDX-License-Identifier: MIT
#include <atomic>
#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include <sstream>
#include <chrono>

namespace neon {

struct TraceEvent {
  const char* name;
  const char* cat;
  char ph; // 'B' begin, 'E' end, 'i' instant
  uint64_t ts_us;
  uint32_t tid;
};

class Telemetry {
 public:
  uint64_t commands() const { return commands_.load(std::memory_order_relaxed); }
  uint64_t errors() const { return errors_.load(std::memory_order_relaxed); }
 public:
  static Telemetry& instance() { static Telemetry t; return t; }

  // High-level counters (no external deps)
  void incCommands() { commands_.fetch_add(1, std::memory_order_relaxed); }
  void incErrors() { errors_.fetch_add(1, std::memory_order_relaxed); }

  // Prometheus exposition (text/plain)
  std::string prometheusText(uint64_t vfs_files, uint64_t vfs_dirs, uint64_t mem_used, uint64_t mem_cap) {
    std::ostringstream o;
    o << "# TYPE neon_commands_total counter
neon_commands_total " << commands_.load() << "\n";
    o << "# TYPE neon_errors_total counter\nneon_errors_total " << errors_.load() << "\n";
    o << "# TYPE neon_vfs_files gauge\nneon_vfs_files " << vfs_files << "\n";
    o << "# TYPE neon_vfs_dirs gauge\nneon_vfs_dirs " << vfs_dirs << "\n";
    o << "# TYPE neon_memory_bytes_used gauge\nneon_memory_bytes_used " << mem_used << "\n";
    o << "# TYPE neon_memory_bytes_capacity gauge\nneon_memory_bytes_capacity " << mem_cap << "\n";
    return o.str();
  }

  // Trace (Chrome trace event format)
  void traceBegin(const char* name, const char* cat) { record(name, cat, 'B'); }
  void traceEnd(const char* name, const char* cat) { record(name, cat, 'E'); }
  void traceInstant(const char* name, const char* cat) { record(name, cat, 'i'); }

  std::string dumpTraceJson() {
    std::lock_guard<std::mutex> g(mu_);
    std::ostringstream o;
    o << "{\"traceEvents\":[";
    for (size_t i=0;i<trace_.size();++i){
      if (i) o << ",";
      auto &e = trace_[i];
      o << "{\"name\":\"" << e.name << "\",\"cat\":\"" << e.cat << "\",\"ph\":\"" << e.ph
        << "\",\"ts\":" << e.ts_us << ",\"tid\":" << e.tid << "}";
    }
    o << "]}";
    return o.str();
  }

 private:
  std::atomic<uint64_t> commands_{0};
  std::atomic<uint64_t> errors_{0};
  std::mutex mu_;
  std::vector<TraceEvent> trace_;

  void record(const char* name, const char* cat, char ph){
    using namespace std::chrono;
    auto now = time_point_cast<microseconds>(steady_clock::now()).time_since_epoch().count();
    uint32_t tid = static_cast<uint32_t>(::getpid()); // best-effort on POSIX; OK as logical thread id here
    std::lock_guard<std::mutex> g(mu_);
    trace_.push_back({name, cat, ph, static_cast<uint64_t>(now), tid});
  }
};

} // namespace neon
