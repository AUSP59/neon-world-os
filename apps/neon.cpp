
static int neon_version_json(){
  extern const char* neon_c_version();
  const char* v = neon_c_version ? neon_c_version() : "unknown";
  printf("{\"name\":\"NEON-WORLD-OS\",\"version\":\"%s\"}\n", v);
  return 0;
}

static void neon_sig_handler(int sig){
  fprintf(stderr, "neon: received signal %d, exiting gracefully\n", sig);
}
static void neon_install_sighandlers(){
  signal(SIGINT, neon_sig_handler);
  signal(SIGTERM, neon_sig_handler);
}
#include <csignal>
#include <map>
#include <sstream>

// --- Help JSON (static contract) ---
static int neon_print_help_json(){
  const char* json = R"JSON({
    "name":"neon",
    "version":"1",
    "options":[
      {"flag":"--help","desc":"Show help"},
      {"flag":"--version","desc":"Show version"},
      {"flag":"--about-json","desc":"Print build metadata JSON"},
      {"flag":"--help-json","desc":"Print this help in JSON"},
      {"flag":"--trace-out","arg":"<file>","desc":"Write Chrome-trace JSON to file"},
      {"flag":"--self-test-backtrace","desc":"Emit a short backtrace to stderr (POSIX)"}
    ]
  })JSON";
  printf("%s\n", json);
  return 0;
}

static int neon_self_test_backtrace(){
#ifdef __unix__
  void* buf[32];
  int n = backtrace(buf, 32);
  backtrace_symbols_fd(buf, n, STDERR_FILENO);
  return 0;
#else
  fprintf(stderr, "Backtrace not supported on this platform.\n");
  return 0;
#endif
}
#ifdef __unix__
#include <execinfo.h>
#include <unistd.h>
#endif

// --- About JSON printer (injected) ---
static int neon_print_about_json(){
  printf("{\"name\":\"NEON-WORLD-OS\",\"version\":\"%s\",\"git\":\"%s\",\"compiler\":\"%s\",\"build_type\":\"%s\",\"std\":\"%s\",\"date\":\"%s\"}\n",
    neon::build::version, neon::build::git, neon::build::compiler, neon::build::type, neon::build::std, neon::build::date);
  return 0;
}
#include "neon/telemetry.hpp"
// SPDX-License-Identifier: MIT
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "neon/scheduler.hpp"
#include "neon/vfs.hpp"
#include "neon/shell.hpp"
#include "neon/common.hpp"
#include "neon/trace.hpp"
#include "neon/build_info.hpp"
#include <fstream>
#include <filesystem>
#include <cstdlib>

int main(){
  for (int i=1;i<argc;i++){ if (std::string(argv[i])=="--version-json") return neon_version_json(); }
  for (int i=1;i<argc;i++){ if (std::string(argv[i])=="print-sample-config") return neon_print_sample_config(); if (std::string(argv[i])=="config" && i+1<argc && std::string(argv[i+1])=="--validate" && i+2<argc && std::string(argv[i+2])=="--json") return neon_config_validate_json(); }
  for (int i=1;i<argc;i++){ if (std::string(argv[i])=="--install-sighandlers"){ neon_install_sighandlers(); }}
  for (int i=1;i<argc;i++){ if (std::string(argv[i])=="doctor" && i+1<argc && std::string(argv[i+1])=="--json") return neon_doctor_json(); }
  for (int i=1;i<argc;i++){ if (std::string(argv[i])=="--help-json") return neon_print_help_json(); if (std::string(argv[i])=="--trace-out" && i+1<argc){   std::ofstream ofs(argv[++i]); extern std::string neon_trace_dump_json_for_cli();   ofs << neon_trace_dump_json_for_cli(); return ofs.good()?0:1; } }
  for (int i=1;i<argc;i++){ if (std::string(argv[i])=="--self-test-backtrace") return neon_self_test_backtrace(); }
  for (int i=1;i<argc;i++){ if (std::string(argv[i])=="--about-json") return neon_print_about_json(); }
  for (int i=1;i<argc;i++){    if (std::string(argv[i])=="metrics"){ /* constructed best-effort objects */ } }
  for (int i=1;i<argc;i++){ if (std::string(argv[i])=="--version"){ printf("NEON-WORLD-OS %s\ncommit=%s\ncompiler=%s\n", neon::build::version, neon::build::git, neon::build::compiler); return 0; } }
  using namespace neon;
  std::cout << "NEON-WORLD-OS v" << std::string(kVersion) << " starting...\n";

  Scheduler sched;
  Vfs vfs;
  if (const char* tf = std::getenv("NEON_TRACE")) {
    neon::Trace::Instance().Start(tf);
  }

  Shell sh(sched, vfs);
  apply_config(sh);

/* SELFTEST */
if (selftest){
  int rc = 0;
  try {
    // basic VFS operations
    vfs.Mkdir("/st");
    vfs.Write("/st/a.txt", "ok");
    auto r = vfs.Read("/st/a.txt");
    if (!r.has_value() || *r != "ok") rc = 2;
    // shell JSON ps
    if (rc==0){
      auto out = sh.Exec("mode json");
      (void)out;
      auto ps = sh.Exec("ps");
      if (ps.find("{")==std::string::npos && ps.find("[")==std::string::npos) rc = 2;
    }
  } catch(...) { rc = 2; }
  return rc;
}


  if (!multi.empty()) {
    if (json) sh.Exec("mode json");
    int errc = 0; std::string tmp; std::istringstream ss(multi);
    while (std::getline(ss, tmp, ';')) { if (tmp.empty()) continue; auto out = sh.Exec(tmp); if (!out.empty()) std::cout << out << "\n"; if (out=="err") errc = 2; }
    return errc;
  }


/* STDIN BATCH */
if (!tty && cmd.empty() && script.empty() && multi.empty()){
  std::string line; int errc = 0;
  while (std::getline(std::cin, line)){
    if (line.empty()) continue;
    auto out = sh.Exec(line);
    if (!out.empty()) std::cout << out << "\n";
    if (out == "err") errc = 2;
  }
  return errc;
}
  if (!script.empty()) {
    std::ifstream in(script);
    if (!in.is_open()) { std::cerr << "cannot open script: " << script << "\n"; return 1; }
    if (json) sh.Exec("mode json");
    std::string line;

  const char* green = "\033[32m";
  const char* reset = "\033[0m";
  bool tty = isatty(0) && isatty(1);
  const char* ncenv = std::getenv("NO_COLOR"); if (ncenv) nocolor = true;
  std::string prompt = (tty && !nocolor) ? std::string(green) + "neon> " + reset : "neon> ";
 int errc = 0;
    while (std::getline(in, line)) {
      if (line.empty()) continue;
      std::string out = sh.Exec(line);
      if (!out.empty()) std::cout << out << "\n";
      if (out == "err") errc = 2;
    }
    return errc;
  }


  vfs.Mkdir("/etc");
  vfs.Touch("/etc/motd");
  vfs.Write("/etc/motd", "Welcome to NEON-WORLD-OS\n(es ? "Escribe 'help' para ver comandos" : "Type 'help' to list commands") (plugins supported: `plugin load <lib>`; trace via NEON_TRACE=trace.json) (savefs/loadfs/tree available).");

  std::cout << vfs.Read("/etc/motd").value() << "\n";

  bool running = true;
  std::thread loop([&]{
    while (running) {
      sched.RunOnce(std::chrono::milliseconds(10));
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  });

  std::string line;

  const char* green = "\033[32m";
  const char* reset = "\033[0m";
  bool tty = isatty(0) && isatty(1);
  const char* ncenv = std::getenv("NO_COLOR"); if (ncenv) nocolor = true;
  std::string prompt = (tty && !nocolor) ? std::string(green) + "neon> " + reset : "neon> ";

  while (true) {
    std::cout << prompt;
    if (!std::getline(std::cin, line)) break;
    if (line == "exit" || line == "quit") break;
    std::string out = sh.Exec(line);
    if (!out.empty()) std::cout << out << "\n";
  }

  running = false;
  loop.join();
  neon::Trace::Instance().Stop();
  std::cout << "Bye.\n";
  return 0;
}


// --- Extended commands: metrics / tracedump ---
static void neon_print_metrics(neon::Vfs& vfs, neon::MemoryManager& mm){
  // best-effort counts from VFS (walk)
  uint64_t files=0, dirs=0;
  vfs.walk("/", [&](const std::string& p, bool is_dir){
    if (is_dir) ++dirs; else ++files;
    return true;
  });
  auto used = mm.bytes_used();
  auto cap  = mm.capacity();
  auto txt = neon::Telemetry::instance().prometheusText(files, dirs, used, cap);
  fwrite(txt.data(), 1, txt.size(), stdout);
  fputc('\n', stdout);
}


// --- Minimal INI config loader (no deps) ---
// Precedence: $NEON_CONFIG_FILE > $XDG_CONFIG_HOME/neon/neon.conf > ~/.config/neon/neon.conf > /etc/neon/neon.conf
struct NeonConfig { std::string file; std::map<std::string,std::string> kv; };
static std::string neon_trim(const std::string& s){
  const char* ws=" \t\r\n"; auto b=s.find_first_not_of(ws); if (b==std::string::npos) return "";
  auto e=s.find_last_not_of(ws); return s.substr(b, e-b+1);
}
static NeonConfig neon_load_config(){
  NeonConfig cfg;
#ifdef _WIN32
  const char* home = std::getenv("USERPROFILE");
  std::string etc = ""; // N/A
#else
  const char* home = std::getenv("HOME");
  std::string etc = "/etc/neon/neon.conf";
#endif
  const char* override = std::getenv("NEON_CONFIG_FILE");
  std::vector<std::string> cands;
  if (override && *override) cands.push_back(override);
  const char* xdg = std::getenv("XDG_CONFIG_HOME");
  if (xdg && *xdg) cands.push_back(std::string(xdg)+"/neon/neon.conf");
  if (home && *home) cands.push_back(std::string(home)+"/.config/neon/neon.conf");
  if (!etc.empty()) cands.push_back(etc);
  for (const auto& p: cands){
    std::ifstream f(p); if (!f.good()) continue;
    cfg.file = p;
    std::string line;
    while (std::getline(f, line)){
      auto L = neon_trim(line);
      if (L.empty() || L[0]=='#' || L[0]==';' || L[0]=='[') continue;
      auto eq = L.find('=');
      if (eq==std::string::npos) continue;
      auto k = neon_trim(L.substr(0,eq));
      auto v = neon_trim(L.substr(eq+1));
      for (auto& c: k) c = (char)std::tolower(c);
      cfg.kv[k]=v;
    }
    break;
  }
  return cfg;
}
static bool neon_cfg_telemetry_off(const NeonConfig& cfg){
  auto it = cfg.kv.find("telemetry");
  if (it==cfg.kv.end()) return false;
  auto v = it->second; std::string L; L.reserve(v.size());
  for (auto c: v){ L.push_back((char)std::tolower(c)); }
  return (L=="off" || L=="false" || L=="0" || L=="disabled");
}

// Doctor JSON
static int neon_doctor_json(){
  NeonConfig cfg = neon_load_config();
  bool env_off = std::getenv("NEON_NO_TELEMETRY")!=nullptr;
  bool eff_off = env_off || neon_cfg_telemetry_off(cfg);
  std::ostringstream o;
  o << "{";
  o << "\"name\":\"NEON-WORLD-OS\",";
  extern const char* neon_c_version(); // from C shim
  o << "\"version\":\"" << (neon_c_version? neon_c_version(): "unknown") << "\",";
  o << "\"telemetry_effective\":" << (eff_off? "true":"false") << ",";
  o << "\"config\":{";
  o << "\"file\":" << (cfg.file.empty()? "null": ("\""+cfg.file+"\"")) << ",";
  auto it = cfg.kv.find("telemetry");
  o << "\"telemetry\":" << (it==cfg.kv.end()? "null": ("\""+it->second+"\""));
  o << "},";
  o << "\"env\":{";
  o << "\"NEON_NO_TELEMETRY\":" << (env_off? "true":"false");
  o << "}";
  o << "}";
  printf("%s\n", o.str().c_str());
  return 0;
}

// --- Print sample config to stdout
static int neon_print_sample_config(){
  printf("# NEON-WORLD-OS sample configuration (INI-like, sectionless)\n");
  printf("# telemetry = on|off\n");
  printf("telemetry = on\n");
  return 0;
}

// --- Validate config and output JSON result
static int neon_config_validate_json(){
  NeonConfig cfg = neon_load_config();
  bool ok = true;
  std::string err = "";
  auto it = cfg.kv.find("telemetry");
  if (it!=cfg.kv.end()){
    std::string v = it->second; for (auto& c: v) c=(char)std::tolower(c);
    if (!(v=="on"||v=="off"||v=="true"||v=="false"||v=="0"||v=="1")){
      ok = false; err = "invalid telemetry value";
    }
  }
  std::ostringstream o;
  o << "{";
  o << "\"file\":" << (cfg.file.empty()? "null": ("\""+cfg.file+"\"")) << ",";
  o << "\"ok\":" << (ok? "true":"false") << ",";
  o << "\"error\":" << (err.empty()? "null": ("\""+err+"\"")) << ",";
  o << "\"keys\":" << cfg.kv.size();
  o << "}";
  printf("%s\n", o.str().c_str());
  return ok? 0: 1;
}
