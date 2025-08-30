cmake_minimum_required(VERSION 3.22)
\1
# ---- Ultra build options ----
option(NEON_CXX23 "Use C++23 if available" ON)
option(NEON_ENABLE_LTO "Enable Interprocedural Optimization / LTO" OFF)
option(NEON_ENABLE_PGO_GEN "Enable PGO profile generation" OFF)
option(NEON_ENABLE_PGO_USE "Enable PGO profile use (requires profile data)" OFF)
option(NEON_REPRODUCIBLE "Enable reproducible build settings" ON)

if (NEON_CXX23)
  set(CMAKE_CXX_STANDARD 23)
endif()

include(CheckIPOSupported)
if (NEON_ENABLE_LTO)
  check_ipo_supported(RESULT ipo_ok OUTPUT ipo_err)
  if (ipo_ok)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
  else()
    message(WARNING "IPO/LTO not supported: ${ipo_err}")
  endif()
endif()

if (NEON_ENABLE_PGO_GEN AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-fprofile-generate)
  add_link_options(-fprofile-generate)
elseif (NEON_ENABLE_PGO_USE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-fprofile-use -fprofile-correction)
  add_link_options(-fprofile-use -fprofile-correction)
endif()

if (NEON_REPRODUCIBLE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-ffile-prefix-map=${CMAKE_SOURCE_DIR}=. -fdebug-prefix-map=${CMAKE_SOURCE_DIR}=.)
  add_link_options(-Wl,--build-id=sha1)
endif()

# ---- Build info header ----
include(CMakePrintHelpers)
set(BUILD_DATE "$ENV{{SOURCE_DATE_EPOCH}}")
if (NOT BUILD_DATE)
  string(TIMESTAMP BUILD_DATE "%Y-%m-%dT%H:%M:%SZ" UTC)
endif()

find_package(Git QUIET)
if (GIT_FOUND)
  execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short=12 HEAD
                  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                  OUTPUT_VARIABLE GIT_COMMIT OUTPUT_STRIP_TRAILING_WHITESPACE
                  ERROR_QUIET)
endif()

set(NEON_BUILD_COMPILER "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
set(NEON_BUILD_TYPE "${CMAKE_BUILD_TYPE}")
set(NEON_STD "${CMAKE_CXX_STANDARD}")

configure_file(${CMAKE_SOURCE_DIR}/include/neon/build_info.hpp.in
               ${CMAKE_BINARY_DIR}/generated/neon/build_info.hpp @ONLY)
include_directories(${CMAKE_BINARY_DIR}/generated)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTING "Build tests" ON)
option(BUILD_BENCHMARKS "Build benchmarks" ON)

option(NEON_ENABLE_TELEMETRY "Enable built-in metrics and trace file emission" ON)
option(NEON_ENABLE_DOCS_SPHINX "Enable Sphinx doc build target" OFF)
option(NEON_BENCH_GUARD "Fail CI if core benchmarks regress beyond thresholds" OFF)
option(BUILD_FUZZ "Build fuzz targets" OFF)
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

if (MSVC)
  add_compile_options(/W4 /permissive-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if (ENABLE_ASAN AND NOT MSVC)
  add_link_options(-fsanitize=address)
  add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
endif()
if (ENABLE_UBSAN AND NOT MSVC)
  add_link_options(-fsanitize=undefined)
  add_compile_options(-fsanitize=undefined -fno-omit-frame-pointer)
endif()

include(GNUInstallDirs)

add_library(neon STATIC
  src/memory.cpp
  src/scheduler.cpp
  src/vfs.cpp
  src/shell.cpp
  src/util.cpp
)
target_include_directories(neon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(neon apps/neon.cpp)
target_link_libraries(neon PRIVATE neon)

# ---- Tests ----
if (BUILD_TESTING)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
  enable_testing()
  add_executable(neon_tests
    tests/test_vfs.cpp
    tests/test_memory.cpp
    tests/test_scheduler.cpp
    tests/test_shell.cpp
    tests/test_util.cpp)
  target_link_libraries(neon_tests PRIVATE neon GTest::gtest_main)
  include(GoogleTest)
  gtest_discover_tests(neon_tests)
endif()

# ---- Benchmarks ----
if (BUILD_BENCHMARKS)
  include(FetchContent)
  FetchContent_Declare(
    googlebenchmark
    URL https://github.com/google/benchmark/archive/refs/tags/v1.8.3.zip
  )
  FetchContent_MakeAvailable(googlebenchmark)
  add_executable(neon_bench benchmarks/bench_vfs.cpp)
  target_link_libraries(neon_bench PRIVATE neon benchmark::benchmark)
endif()

# ---- Fuzz ----
if (BUILD_FUZZ AND NOT MSVC)
  add_executable(shell_fuzz fuzz/shell_fuzz.cpp)
  target_link_options(shell_fuzz PRIVATE -fsanitize=fuzzer,address,undefined)
  target_compile_options(shell_fuzz PRIVATE -fsanitize=fuzzer,address,undefined)
  target_link_libraries(shell_fuzz PRIVATE neon)
endif()

# ---- Doxygen (API docs) ----
find_package(Doxygen QUIET)
if (Doxygen_FOUND)
  set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/api/Doxyfile)
  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif()

# ---- Install & CMake package ----
install(TARGETS neon neon
  EXPORT neonTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/NEONWorldOSConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/NEONWorldOSConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/NEONWorldOSConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NEONWorldOS
)
install(EXPORT neonTargets
        FILE neonTargets.cmake
        NAMESPACE neon::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NEONWorldOS)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/NEONWorldOSConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/NEONWorldOSConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/NEONWorldOS)

# ---- Packaging ----
include(CPack)
set(CPACK_PACKAGE_NAME "neon-world-os")
set(CPACK_PACKAGE_VENDOR "NEON")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "TGZ;ZIP")


# ---- pkg-config (.pc) ----
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pkgconfig/neon-world-os.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/neon-world-os.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/neon-world-os.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# Optional clang-tidy (enable if available)
find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if (CLANG_TIDY_EXE)
  set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
endif()

# Format target
add_custom_target(format
  COMMAND ${CMAKE_SOURCE_DIR}/tools/dev/format.sh
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# ---- Hardening & Tooling ----
option(ENABLE_LTO "Enable Link Time Optimization" ON)
find_program(CCACHE_PROGRAM ccache)
if (CCACHE_PROGRAM)
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-fPIC -fstack-protector-strong -D_FORTIFY_SOURCE=2)
  if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_link_options(-Wl,-z,relro,-z,now)
  endif()
endif()
if (ENABLE_LTO AND CMAKE_BUILD_TYPE STREQUAL "Release")
  include(CheckIPOSupported)
  check_ipo_supported(RESULT ipo_supported OUTPUT error)
  if (ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
  endif()
endif()

# Logging header is header-only
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/neon/log.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/neon)


# ---- Compiler Warnings & Visibility ----
if (MSVC)
  add_compile_options(/W4 /permissive- /EHsc)
  # Control Flow Guard & Spectre mitigations where available
  add_link_options(/guard:cf)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion -fvisibility=hidden -fPIC)
  if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_link_options(-pie)
  endif()
endif()
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Precompiled headers (optional, speeds up builds)
option(NEON_PCH "Enable precompiled headers" OFF)
if (NEON_PCH)
  target_precompile_headers(neon PRIVATE <vector> <string> <memory>)
endif()


# ---- Shared/static toggle ----
option(NEON_BUILD_SHARED "Build neon as shared library" OFF)
get_target_property(_neon_type neon TYPE)
if (DEFINED _neon_type)
  # Re-create if needed is complex; we assume 'neon' already defined.
  if (NEON_BUILD_SHARED AND _neon_type STREQUAL "STATIC_LIBRARY")
    message(STATUS "NEON_BUILD_SHARED requested; ensure downstream CI matrix builds shared variant too.")
  endif()
endif()

# ---- Install manpage & completions ----
install(FILES ${CMAKE_SOURCE_DIR}/docs/man/neon.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
install(FILES ${CMAKE_SOURCE_DIR}/scripts/completions/neon.bash DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/bash-completion/completions RENAME neon)
install(FILES ${CMAKE_SOURCE_DIR}/scripts/completions/neon.zsh DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/zsh/site-functions RENAME _neon)

# C API pkg-config
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pkgconfig/neon-world-os-c.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/neon-world-os-c.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/neon-world-os-c.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)


# Uninstall target
configure_file(${CMAKE_SOURCE_DIR}/cmake/uninstall.cmake.in ${CMAKE_BINARY_DIR}/uninstall.cmake IMMEDIATE @ONLY)
add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/uninstall.cmake)

# ---- Build Policy (Werror/PGO/Reproducible) ----
option(NEON_WERROR "Treat warnings as errors" ON)
option(NEON_PGO_GENERATE "Enable -fprofile-generate" OFF)
option(NEON_PGO_USE "Enable -fprofile-use -fprofile-correction" OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  if (NEON_WERROR)
    add_compile_options(-Werror)
  endif()
  # Reproducible paths in debug info
  add_compile_options(-ffile-prefix-map=${CMAKE_SOURCE_DIR}=. -ffile-prefix-map=${CMAKE_BINARY_DIR}=build)
  if (NEON_PGO_GENERATE)
    add_compile_options(-fprofile-generate)
    add_link_options(-fprofile-generate)
  endif()
  if (NEON_PGO_USE)
    add_compile_options(-fprofile-use -fprofile-correction)
    add_link_options(-fprofile-use -fprofile-correction)
  endif()
endif()

# Build info header (configured at configure-time)
string(TIMESTAMP NEON_BUILD_TIMESTAMP "%Y-%m-%dT%H:%M:%SZ" UTC)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/build_info.hpp.in
               ${CMAKE_CURRENT_BINARY_DIR}/generated/neon/build_info.hpp @ONLY)

# Ensure apps/neon sees generated build_info header
get_target_property(_neon_cli_targets neon_cli TYPE)
if (TARGET neon)
  target_include_directories(neon PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)
endif()


# Separate C API tests (link to shared C API lib if available; otherwise to neon)
add_executable(neon_c_tests tests/test_c_api.cpp)
target_link_libraries(neon_c_tests PRIVATE neonc GTest::gtest_main)
gtest_discover_tests(neon_c_tests)

option(BUILD_PYTHON_BINDINGS "Build Python bindings" OFF)
if (BUILD_PYTHON_BINDINGS)
  add_subdirectory(bindings/python)
endif()
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/neon/neon.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/neon)

# Ensure ABI versioning for neon (if target exists)
if (TARGET neon)
  set_target_properties(neon PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR} VERSION ${PROJECT_VERSION})
endif()

add_subdirectory(plugins/echo_plus)


# ---- Benchmarks (Google Benchmark) ----
option(BUILD_BENCHMARKS "Build benchmarks" OFF)
if (BUILD_BENCHMARKS)
  find_package(benchmark CONFIG QUIET)
  if (benchmark_FOUND)
    add_executable(neon_bench benchmarks/vfs_bench.cpp)
    target_link_libraries(neon_bench PRIVATE neon benchmark::benchmark)
  else()
    message(STATUS "benchmark not found; skipping neon_bench")
  endif()
endif()

# Install export header
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/neon/export.hpp DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/neon)


# Hardened builds: disable runtime plugins if desired
option(NEON_ENABLE_PLUGINS "Enable runtime plugin loader" ON)
if (NEON_ENABLE_PLUGINS)
  add_compile_definitions(NEON_ENABLE_PLUGINS=1)
else()
  add_compile_definitions(NEON_ENABLE_PLUGINS=0)
endif()


# SBOM custom target
add_custom_target(sbom
  COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH= tools/sbom/generate.py
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# ---- Reproducible builds (basic) ----
if(DEFINED ENV{SOURCE_DATE_EPOCH})
  message(STATUS "SOURCE_DATE_EPOCH detected: $ENV{SOURCE_DATE_EPOCH}")
  add_compile_definitions(SOURCE_DATE_EPOCH=$ENV{SOURCE_DATE_EPOCH})
  # Avoid embedding __DATE__/__TIME__
  add_compile_definitions(__DATE__="redacted" __TIME__="redacted")
endif()


# ---- Amalgamation single-header ----
add_custom_target(amalgamate
  COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH= tools/dev/amalgamate.py
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
install(FILES ${CMAKE_SOURCE_DIR}/single_include/neon_world_os.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# ---- Header self-containment compile check ----
add_custom_target(header_selfcheck
  COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH= tools/dev/gen_header_build.py
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
file(GLOB HEADER_TUS CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/tests/headers/*.cpp)
if (HEADER_TUS)
  add_library(neon_headers_check OBJECT ${HEADER_TUS})
  target_include_directories(neon_headers_check PRIVATE ${CMAKE_SOURCE_DIR}/include)
  # Just compile; do not link
endif()

# ---- Windows DEF exports for neonc ----
if (WIN32 AND TARGET neon_c)
  target_link_options(neon_c PRIVATE "/DEF:${CMAKE_SOURCE_DIR}/packaging/def/neonc.def")
endif()

# ---- RPATH sanity (non-mac static; macOS @rpath) ----
if(APPLE)
  set(CMAKE_INSTALL_RPATH "@loader_path/../lib")
elseif(UNIX)
  set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif()


# Thread-safety toggle for VFS
option(NEON_THREAD_SAFE "Enable internal mutex guards for VFS public APIs" ON)
if (NEON_THREAD_SAFE)
  add_compile_definitions(NEON_THREAD_SAFE=1)
else()
  add_compile_definitions(NEON_THREAD_SAFE=0)
endif()


# Exceptions/RTTI toggles (off for ultra-small builds)
option(NEON_DISABLE_EXCEPTIONS "Build with -fno-exceptions" OFF)
option(NEON_DISABLE_RTTI "Build with -fno-rtti" OFF)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  if (NEON_DISABLE_EXCEPTIONS) add_compile_options(-fno-exceptions) endif()
  if (NEON_DISABLE_RTTI) add_compile_options(-fno-rtti) endif()
endif()

# Install C++ pkg-config
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pkgconfig/neon-world-os-cpp.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/neon-world-os-cpp.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/neon-world-os-cpp.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# Doxygen docs target
find_package(Doxygen QUIET)
if (DOXYGEN_FOUND)
  add_custom_target(docs
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/docs/api/Doxyfile
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating API docs with Doxygen")
endif()


# ---- Hardening flags (best-effort) ----
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_definitions(_FORTIFY_SOURCE=2)
  add_compile_options(-fstack-protector-strong -fno-omit-frame-pointer)
  if (UNIX AND NOT APPLE)
    add_link_options(-Wl,-z,relro -Wl,-z,now)
  endif()
endif()

# ---- Verify meta target ----
add_custom_target(verify
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target header_selfcheck
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target sbom
  COMMENT "Running tests, header self-check, and SBOM generation")


# ---- Performance build toggles ----
option(ENABLE_LTO "Enable Link-Time Optimization" OFF)
option(ENABLE_UNITY_BUILD "Enable Unity builds for faster compilation" OFF)

if (ENABLE_LTO)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT ipo_ok OUTPUT out)
  if (ipo_ok)
    set_property(TARGET neon PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    if (TARGET neon_c) set_property(TARGET neon_c PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE) endif()
  else()
    message(WARNING "IPO/LTO not supported: ${out}")
  endif()
endif()

if (ENABLE_UNITY_BUILD)
  set_target_properties(neon PROPERTIES UNITY_BUILD ON)
  if (TARGET neon_c) set_target_properties(neon_c PROPERTIES UNITY_BUILD ON) endif()
endif()


# Install C pkg-config
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pkgconfig/neon-world-os-c.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/neon-world-os-c.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/neon-world-os-c.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)


# Formatting helper (optional)
add_custom_target(format
  COMMAND ${CMAKE_COMMAND} -E echo "Formatting (requires clang-format in PATH)"
  COMMAND ${CMAKE_COMMAND} -E env bash -c "command -v clang-format >/dev/null 2>&1 && clang-format -i $(git ls-files '*.h' '*.hpp' '*.c' '*.cc' '*.cpp' '*.cxx' || true) || true"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# SPDX check
add_custom_target(spdx_check
  COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH= ${CMAKE_SOURCE_DIR}/tools/dev/spdx_check.py
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# Add to verify (if target exists)
add_custom_target(verify_plus
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target verify
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target spdx_check
  COMMENT "Verify + SPDX")


# Optional permission enforcement in VFS writes
option(NEON_ENFORCE_PERMS "Enforce VFS write permission bits (user write)" OFF)
if (NEON_ENFORCE_PERMS)
  add_compile_definitions(NEON_ENFORCE_PERMS=1)
endif()

# Export basic package config for integration
include(CMakePackageConfigHelpers)

# Export alias for consumers
add_library(Neon::neon ALIAS neon)


# ---- Hardened build flags (safe, best-effort) ----
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo")
  add_compile_options(-D_FORTIFY_SOURCE=3 -fstack-protector-strong -fstack-clash-protection -fPIE -fno-plt)
  add_link_options(-Wl,-z,relro,-z,now -pie)
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND CMAKE_BUILD_TYPE MATCHES "Debug")
  add_compile_options(-D_GLIBCXX_ASSERTIONS)
endif()



# ---- Man page install ----
include(GNUInstallDirs)
if (EXISTS "${CMAKE_SOURCE_DIR}/docs/man/neon.1")
  install(FILES ${CMAKE_SOURCE_DIR}/docs/man/neon.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
endif()


# ---- CLI golden smoke test ----
if (BUILD_TESTING)
  add_test(NAME cli_help COMMAND $<TARGET_FILE:neon> --help)
endif()


# ---- Pedantic mode / Werror in CI ----
option(NEON_PEDANTIC "Enable -Werror for pedantic builds" OFF)
if (NEON_PEDANTIC OR DEFINED ENV{CI})
  if (MSVC)
    add_compile_options(/W4 /WX)
  else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
  endif()
endif()


# ---- CCache integration ----
option(NEON_USE_CCACHE "Use ccache if available" ON)
if (NEON_USE_CCACHE)
  find_program(CCACHE_PROGRAM ccache)
  if (CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER   ${CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    message(STATUS "ccache enabled: ${CCACHE_PROGRAM}")
  endif()
endif()


# ---- Visibility control (optional) ----
option(NEON_DEFAULT_HIDDEN "Use -fvisibility=hidden on lib targets (reduce symbol exports)" OFF)
if (NEON_DEFAULT_HIDDEN AND NOT MSVC)
  add_compile_options(-fvisibility=hidden -fvisibility-inlines-hidden)
endif()


# ---- Optional C API shim ----
option(NEON_BUILD_C_API "Build C language API shim" ON)
if (NEON_BUILD_C_API AND TARGET neon)
  add_library(neon_c src/api_c.cpp)
  target_link_libraries(neon_c PUBLIC neon)
  target_include_directories(neon_c PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
  set_target_properties(neon_c PROPERTIES OUTPUT_NAME neon_c)
  install(TARGETS neon_c
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
  install(FILES ${CMAKE_SOURCE_DIR}/include/neon_c.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  # pkg-config for C consumers
  configure_file(${CMAKE_SOURCE_DIR}/packaging/pkgconfig/neon-world-os-c.pc.in
                 ${CMAKE_BINARY_DIR}/neon-world-os-c.pc @ONLY)
  install(FILES ${CMAKE_BINARY_DIR}/neon-world-os-c.pc
          DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()


# ---- C API smoke test ----
if (BUILD_TESTING AND TARGET neon_c)
  add_executable(capi_test tests/c/capi_test.c)
  target_link_libraries(capi_test PRIVATE neon_c)
  add_test(NAME capi_version COMMAND capi_test)
endif()


# ---- CLI JSON about smoke test ----
if (BUILD_TESTING)
  add_test(NAME cli_about_json COMMAND $<TARGET_FILE:neon> --about-json)
endif()


# ---- Trace JSON schema validation ----
if (BUILD_TESTING)
  add_test(NAME trace_json_schema
    COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${CMAKE_SOURCE_DIR}/tools/python
            ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/validate_trace.py
            $<TARGET_FILE:neon> ${CMAKE_SOURCE_DIR}/schemas/trace.schema.json)
endif()


# ---- Version script for neon_c (GNU ld) ----
if (TARGET neon_c AND UNIX AND NOT APPLE)
  target_link_options(neon_c PRIVATE "-Wl,--version-script=/mnt/data/apogee_pass/NEON-WORLD-OS_APOGEE/packaging/linker/neon_c.version")
endif()


# ---- SONAME for C ABI library ----
if (TARGET neon_c)
  set_target_properties(neon_c PROPERTIES SOVERSION 240 VERSION 240.0.0)
endif()

# ---- SONAME guard test (best-effort) ----
if (BUILD_TESTING AND UNIX AND NOT APPLE AND TARGET neon_c)
  add_test(NAME soname_guard
    COMMAND ${CMAKE_COMMAND} -E sh -c "readelf -d $<TARGET_FILE:neon_c> | grep -q SONAME")
endif()


# ---- Help JSON schema validation ----
if (BUILD_TESTING)
  add_test(NAME help_json_schema
    COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${CMAKE_SOURCE_DIR}/tools/python
            ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/validate_help.py
            $<TARGET_FILE:neon> ${CMAKE_SOURCE_DIR}/schemas/help.schema.json)
endif()


# ---- Doctor JSON schema validation ----
if (BUILD_TESTING)
  add_test(NAME doctor_json_schema
    COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${CMAKE_SOURCE_DIR}/tools/python
            ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/validate_doctor.py
            $<TARGET_FILE:neon> ${CMAKE_SOURCE_DIR}/schemas/doctor.schema.json)
endif()


# ---- Config validate JSON schema ----
if (BUILD_TESTING)
  add_test(NAME config_validate_schema
    COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${CMAKE_SOURCE_DIR}/tools/python
            ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/validate_config_validate.py
            $<TARGET_FILE:neon> ${CMAKE_SOURCE_DIR}/schemas/config_validate.schema.json)
  add_test(NAME print_sample_config
    COMMAND $<TARGET_FILE:neon> print-sample-config)
endif()


# ---- End-to-end smoke test ----
if (BUILD_TESTING)
  add_test(NAME e2e_smoke
    COMMAND ${CMAKE_COMMAND} -E env bash ${CMAKE_SOURCE_DIR}/tools/e2e_smoke.sh $<TARGET_FILE:neon>)
endif()


# ---- Install smoke test ----
if (BUILD_TESTING)
  add_test(NAME install_smoke
    COMMAND ${CMAKE_COMMAND} -E sh -c "cmake --install . --prefix inst && ./inst/bin/neon --version")
  set_tests_properties(install_smoke PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

# ---- Strict warnings toggle ----
option(NEON_STRICT_WARNINGS "Enable strict compiler warnings" ON)
if (NEON_STRICT_WARNINGS)
  if (MSVC)
    add_compile_options(/W4)
  else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion)
  endif()
endif()

# ---- CMake options introspection (JSON) ----
if (BUILD_TESTING)
  add_test(NAME cmake_options_introspect
    COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${CMAKE_SOURCE_DIR}/tools/python
            ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/cmake_options_to_json.py
            ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/schemas/cmake_options.schema.json)
endif()

# ---- C API smoke (build + run) ----
if (BUILD_TESTING AND TARGET neon_c)
  add_executable(c_api_smoke tests/c_api_smoke.c)
  target_link_libraries(c_api_smoke PRIVATE neon_c)
  add_test(NAME c_api_smoke_run COMMAND c_api_smoke)
endif()

# ---- Man page ----
install(FILES ${CMAKE_SOURCE_DIR}/man/neon.1 DESTINATION share/man/man1)


# ---- Version JSON schema validation ----
if (BUILD_TESTING)
  add_test(NAME version_json_schema
    COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${CMAKE_SOURCE_DIR}/tools/python
            ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/validate_version.py
            $<TARGET_FILE:neon> ${CMAKE_SOURCE_DIR}/schemas/version.schema.json)
endif()
