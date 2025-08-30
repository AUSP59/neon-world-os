#ifndef NEON_C_API_H
#define NEON_C_API_H
#ifdef __cplusplus
extern "C" {
#endif

// Version & build info
const char* neon_c_version();
const char* neon_c_compiler();
const char* neon_c_build_type();
const char* neon_c_std();
const char* neon_c_build_date();

// Telemetry counters (best-effort, process-global)
unsigned long long neon_c_commands_total();
unsigned long long neon_c_errors_total();

// Trace dump (Chrome trace JSON) - caller must free with neon_c_free
char* neon_c_trace_dump_json();
void neon_c_free(void* p);

#ifdef __cplusplus
}
#endif
#endif
