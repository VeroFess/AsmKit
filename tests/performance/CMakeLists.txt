include(FetchContent)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_USE_BUNDLED_GTEST OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "" FORCE)
FetchContent_Declare(asmkit_google_benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.9.5
  GIT_SHALLOW TRUE)

set(CAPSTONE_BUILD_CSTOOL OFF CACHE BOOL "" FORCE)
set(CAPSTONE_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CAPSTONE_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(CAPSTONE_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
set(CAPSTONE_ARCHITECTURE_DEFAULT OFF CACHE BOOL "" FORCE)
set(CAPSTONE_ARM_SUPPORT ON CACHE BOOL "" FORCE)
set(CAPSTONE_ARM64_SUPPORT ON CACHE BOOL "" FORCE)
set(CAPSTONE_M68K_SUPPORT OFF CACHE BOOL "" FORCE)
set(CAPSTONE_MIPS_SUPPORT OFF CACHE BOOL "" FORCE)
set(CAPSTONE_PPC_SUPPORT OFF CACHE BOOL "" FORCE)
set(CAPSTONE_SPARC_SUPPORT OFF CACHE BOOL "" FORCE)
set(CAPSTONE_SYSZ_SUPPORT OFF CACHE BOOL "" FORCE)
set(CAPSTONE_XCORE_SUPPORT OFF CACHE BOOL "" FORCE)
set(CAPSTONE_X86_SUPPORT ON CACHE BOOL "" FORCE)
set(CAPSTONE_TMS320C64X_SUPPORT OFF CACHE BOOL "" FORCE)
set(CAPSTONE_M680X_SUPPORT OFF CACHE BOOL "" FORCE)
set(CAPSTONE_EVM_SUPPORT OFF CACHE BOOL "" FORCE)
set(CAPSTONE_MOS65XX_SUPPORT OFF CACHE BOOL "" FORCE)
set(CAPSTONE_WASM_SUPPORT ON CACHE BOOL "" FORCE)
set(CAPSTONE_BPF_SUPPORT ON CACHE BOOL "" FORCE)
set(CAPSTONE_RISCV_SUPPORT OFF CACHE BOOL "" FORCE)
set(CAPSTONE_SH_SUPPORT OFF CACHE BOOL "" FORCE)
set(CAPSTONE_TRICORE_SUPPORT OFF CACHE BOOL "" FORCE)
FetchContent_Declare(asmkit_capstone
  GIT_REPOSITORY https://github.com/capstone-engine/capstone.git
  GIT_TAG 5.0.9
  GIT_SHALLOW TRUE)

set(ZYDIS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ZYDIS_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(ZYDIS_BUILD_DOXYGEN OFF CACHE BOOL "" FORCE)
set(ZYDIS_BUILD_MAN OFF CACHE BOOL "" FORCE)
set(ZYDIS_BUILD_SHARED_LIB OFF CACHE BOOL "" FORCE)
set(ZYDIS_FEATURE_FORMATTER OFF CACHE BOOL "" FORCE)
FetchContent_Declare(asmkit_zydis
  GIT_REPOSITORY https://github.com/zyantific/zydis.git
  GIT_TAG v4.1.1
  GIT_SHALLOW TRUE
  GIT_SUBMODULES_RECURSE TRUE)

FetchContent_Declare(asmkit_yaxpeax_x86
  GIT_REPOSITORY https://github.com/iximeow/yaxpeax-x86.git
  GIT_TAG 2.1.1
  GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(asmkit_google_benchmark asmkit_capstone asmkit_zydis asmkit_yaxpeax_x86)

find_program(ASMKIT_CARGO_EXECUTABLE cargo REQUIRED)

set(ASMKIT_YAXPEAX_CAPI_DIR "${CMAKE_CURRENT_BINARY_DIR}/yaxpeax_x86_capi")
set(ASMKIT_YAXPEAX_CAPI_LIB "${ASMKIT_YAXPEAX_CAPI_DIR}/target/release/asmkit_yaxpeax_x86_capi.lib")
set(ASMKIT_YAXPEAX_X86_SOURCE_DIR "${asmkit_yaxpeax_x86_SOURCE_DIR}")
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/yaxpeax_x86_capi/Cargo.toml.in"
  "${ASMKIT_YAXPEAX_CAPI_DIR}/Cargo.toml"
  @ONLY)
file(MAKE_DIRECTORY "${ASMKIT_YAXPEAX_CAPI_DIR}/src")
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/yaxpeax_x86_capi/src/lib.rs"
  "${ASMKIT_YAXPEAX_CAPI_DIR}/src/lib.rs"
  COPYONLY)

add_custom_command(
  OUTPUT "${ASMKIT_YAXPEAX_CAPI_LIB}"
  COMMAND ${CMAKE_COMMAND} -E env
    "CARGO_TARGET_DIR=${ASMKIT_YAXPEAX_CAPI_DIR}/target"
    "CARGO_PROFILE_RELEASE_OPT_LEVEL=3"
    "CARGO_PROFILE_RELEASE_LTO=fat"
    "CARGO_PROFILE_RELEASE_CODEGEN_UNITS=1"
    "CARGO_PROFILE_RELEASE_PANIC=abort"
    "${ASMKIT_CARGO_EXECUTABLE}" build --release --manifest-path "${ASMKIT_YAXPEAX_CAPI_DIR}/Cargo.toml"
  DEPENDS
    "${ASMKIT_YAXPEAX_CAPI_DIR}/Cargo.toml"
    "${ASMKIT_YAXPEAX_CAPI_DIR}/src/lib.rs"
  WORKING_DIRECTORY "${ASMKIT_YAXPEAX_CAPI_DIR}"
  VERBATIM)

add_custom_target(asmkit_yaxpeax_x86_capi_build
  DEPENDS "${ASMKIT_YAXPEAX_CAPI_LIB}")

add_library(asmkit_yaxpeax_x86_capi STATIC IMPORTED GLOBAL)
add_dependencies(asmkit_yaxpeax_x86_capi asmkit_yaxpeax_x86_capi_build)
set_target_properties(asmkit_yaxpeax_x86_capi PROPERTIES
  IMPORTED_LOCATION "${ASMKIT_YAXPEAX_CAPI_LIB}")
if(WIN32)
  set_property(TARGET asmkit_yaxpeax_x86_capi PROPERTY
    INTERFACE_LINK_LIBRARIES "advapi32;bcrypt;ntdll;userenv;ws2_32")
endif()

set(ASMKIT_BENCH_CAPSTONE_TARGET "")
foreach(_asmkit_capstone_target capstone::capstone capstone capstone_static capstone-static)
  if(TARGET ${_asmkit_capstone_target})
    set(ASMKIT_BENCH_CAPSTONE_TARGET ${_asmkit_capstone_target})
    break()
  endif()
endforeach()
if(NOT ASMKIT_BENCH_CAPSTONE_TARGET)
  message(FATAL_ERROR "Capstone FetchContent did not create a known CMake target")
endif()

set(ASMKIT_BENCH_ZYDIS_TARGET "")
foreach(_asmkit_zydis_target Zydis::Zydis Zydis)
  if(TARGET ${_asmkit_zydis_target})
    set(ASMKIT_BENCH_ZYDIS_TARGET ${_asmkit_zydis_target})
    break()
  endif()
endforeach()
if(NOT ASMKIT_BENCH_ZYDIS_TARGET)
  message(FATAL_ERROR "Zydis FetchContent did not create a known CMake target")
endif()

add_executable(asmkit_decode_bench
  decode_bench.cpp)

target_link_libraries(asmkit_decode_bench
  PRIVATE
    asmkit
    benchmark::benchmark_main
    ${ASMKIT_BENCH_CAPSTONE_TARGET}
    ${ASMKIT_BENCH_ZYDIS_TARGET}
    asmkit_yaxpeax_x86_capi)

target_compile_definitions(asmkit_decode_bench
  PRIVATE
    ASMKIT_BENCH_HAS_CAPSTONE=1
    ASMKIT_BENCH_HAS_ZYDIS=1
    ASMKIT_BENCH_HAS_YAXPEAX_X86=1)

set_target_properties(asmkit_decode_bench PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO)

asmkit_enable_strict_warnings(asmkit_decode_bench)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(asmkit_decode_bench PRIVATE -Wno-microsoft-enum-value)
endif()
