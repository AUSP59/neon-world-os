// SPDX-License-Identifier: MIT

// SPDX-License-Identifier: MIT
#pragma once
#include <fstream>
#include <mutex>
#include <string>
#include <chrono>
#include <thread>

namespace neon {

class Trace {
 public:
  static Trace& Instance() {
    static Trace t; return t;
  }
  void Start(const std::string& file){
    std::lock_guard<std::mutex> lk(mu_);
    if (started_) return;
    out_.open(file, std::ios::out | std::ios::trunc);
    if (out_.is_open()) { out_ << "[\n"; started_=true; first_=true; }
  }
  void Event(const char* name, const char* ph){
    std::lock_guard<std::mutex> lk(mu_);
    if (!started_ || !out_.is_open()) return;
    using namespace std::chrono;
    auto ts = duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count();
    if (!first_) out_ << ",\n"; first_=false;
    out_ << "{\"name\":\"" << name << "\",\"ph\":\"" << ph << "\",\"ts\":" << ts << "}";
  }
  void Stop(){
    std::lock_guard<std::mutex> lk(mu_);
    if (started_ && out_.is_open()){ out_ << "\n]\n"; out_.close(); }
    started_=false; first_=true;
  }
 private:
  std::ofstream out_{};
  std::mutex mu_{};
  bool started_=false, first_=true;
};

struct TraceScope {
  const char* name;
  TraceScope(const char* n):name(n){ Trace::Instance().Event(name,"B"); }
  ~TraceScope(){ Trace::Instance().Event(name,"E"); }
};

} // namespace neon
