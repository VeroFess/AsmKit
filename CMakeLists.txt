cmake_minimum_required(VERSION 3.20)
project(AsmKit VERSION 0.1.0 LANGUAGES C)

option(ASMKIT_ENABLE_TEXT "Enable optional instruction text formatting" OFF)
option(ASMKIT_ENABLE_TESTS "Build tests" ON)
option(ASMKIT_FREESTANDING "Avoid libc-dependent code paths" ON)
option(ASMKIT_ENABLE_ASAN "Enable AddressSanitizer for supported compilers" OFF)

if(ASMKIT_ENABLE_ASAN)
  if(MSVC)
    add_compile_options(/fsanitize=address /bigobj)
    add_link_options(/INCREMENTAL:NO)
    get_filename_component(ASMKIT_MSVC_BIN_DIR "${CMAKE_C_COMPILER}" DIRECTORY)
    set(ASMKIT_ASAN_RUNTIME_DLL "${ASMKIT_MSVC_BIN_DIR}/clang_rt.asan_dynamic-x86_64.dll")
    if(NOT EXISTS "${ASMKIT_ASAN_RUNTIME_DLL}")
      message(FATAL_ERROR "ASMKIT_ENABLE_ASAN could not find ${ASMKIT_ASAN_RUNTIME_DLL}")
    endif()
    foreach(_asmkit_flags_var CMAKE_C_FLAGS_DEBUG)
      string(REGEX REPLACE "(^| )[/-]RTC[^ ]*" "" ${_asmkit_flags_var} "${${_asmkit_flags_var}}")
    endforeach()
  elseif(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
  else()
    message(FATAL_ERROR "ASMKIT_ENABLE_ASAN is not supported for ${CMAKE_C_COMPILER_ID}")
  endif()
endif()

function(asmkit_copy_asan_runtime target)
  if(ASMKIT_ENABLE_ASAN AND MSVC)
    add_custom_command(TARGET ${target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ASMKIT_ASAN_RUNTIME_DLL}"
        "$<TARGET_FILE_DIR:${target}>/clang_rt.asan_dynamic-x86_64.dll")
  endif()
endfunction()

set(ASMKIT_CORE_SOURCES
  src/core/asmkit_bounds.c
  src/core/asmkit_easy.c
  src/core/asmkit_engine.c
  src/core/asmkit_error.c
  src/core/asmkit_metadata.c
  src/core/asmkit_workspace.c
  src/decode/asmkit_decode.c
  src/analysis/asmkit_analysis.c
  src/encode/asmkit_emit.c
  src/relocate/asmkit_relocate.c
  src/text/asmkit_text.c
  src/target/x86/asmkit_x86_overlay.c
  src/target/arm/asmkit_arm_overlay.c
  src/target/aarch64/asmkit_aarch64_overlay.c
  src/target/bpf/asmkit_bpf_overlay.c
  src/target/wasm/asmkit_wasm_overlay.c
  src/generated/x86/asmkit_gen_x86_decode.c
  src/generated/x86/asmkit_gen_x86_encode.c
  src/generated/x86/asmkit_gen_x86_semantics.c
  src/generated/arm/asmkit_gen_arm_decode.c
  src/generated/arm/asmkit_gen_arm_encode.c
  src/generated/arm/asmkit_gen_arm_semantics.c
  src/generated/aarch64/asmkit_gen_aarch64_decode.c
  src/generated/aarch64/asmkit_gen_aarch64_encode.c
  src/generated/aarch64/asmkit_gen_aarch64_semantics.c
  src/generated/bpf/asmkit_gen_bpf_decode.c
  src/generated/bpf/asmkit_gen_bpf_encode.c
  src/generated/bpf/asmkit_gen_bpf_semantics.c
  src/generated/wasm/asmkit_gen_wasm_decode.c
  src/generated/wasm/asmkit_gen_wasm_encode.c
  src/generated/wasm/asmkit_gen_wasm_semantics.c)

file(GLOB_RECURSE ASMKIT_PROJECT_HEADERS CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.inc
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/generated/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/generated/*.inc)
set_source_files_properties(${ASMKIT_CORE_SOURCES} PROPERTIES
  OBJECT_DEPENDS "${ASMKIT_PROJECT_HEADERS}")

add_library(asmkit STATIC
  ${ASMKIT_CORE_SOURCES})

target_include_directories(asmkit
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/generated/x86
    ${CMAKE_CURRENT_SOURCE_DIR}/src/generated/arm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/generated/aarch64
    ${CMAKE_CURRENT_SOURCE_DIR}/src/generated/bpf
    ${CMAKE_CURRENT_SOURCE_DIR}/src/generated/wasm)

target_compile_definitions(asmkit PUBLIC ASMKIT_ENABLE_TEXT=$<BOOL:${ASMKIT_ENABLE_TEXT}>)
target_sources(asmkit PRIVATE
  src/generated/x86/asmkit_gen_x86_text.c
  src/generated/arm/asmkit_gen_arm_text.c
  src/generated/aarch64/asmkit_gen_aarch64_text.c
  src/generated/bpf/asmkit_gen_bpf_text.c
  src/generated/wasm/asmkit_gen_wasm_text.c)
if(ASMKIT_FREESTANDING)
  target_compile_definitions(asmkit PUBLIC ASMKIT_FREESTANDING=1)
endif()
set_target_properties(asmkit PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)

if(MSVC)
  target_compile_options(asmkit PRIVATE /W4 /WX)
else()
  target_compile_options(asmkit PRIVATE -Wall -Wextra -Werror -pedantic)
endif()

if(ASMKIT_ENABLE_TESTS)
  enable_testing()
  set(ASMKIT_TEST_SOURCES
    tests/decode/test_decode.c
    tests/api/test_easy.c
    tests/analysis/test_analysis.c
    tests/encode/test_emit.c
    tests/relocate/test_relocate.c
    tests/concurrency/test_concurrency.c
    tests/instruction_corpus/test_instruction_corpus.c
    tests/instruction_corpus/test_instruction_corpus_aarch64.c
    tests/instruction_corpus/test_instruction_corpus_arm_a32.c
    tests/instruction_corpus/test_instruction_corpus_arm_thumb.c
    tests/instruction_corpus/test_instruction_corpus_bpf64.c
    tests/instruction_corpus/test_instruction_corpus_wasm32.c
    tests/instruction_corpus/test_instruction_corpus_x86_16.c
    tests/instruction_corpus/test_instruction_corpus_x86_32.c
    tests/instruction_corpus/test_instruction_corpus_x86_64.c
    tests/all_tests.c)
  set_source_files_properties(${ASMKIT_TEST_SOURCES} PROPERTIES
    OBJECT_DEPENDS "${ASMKIT_PROJECT_HEADERS}")
  add_executable(asmkit_core_tests
    ${ASMKIT_TEST_SOURCES})
  asmkit_copy_asan_runtime(asmkit_core_tests)
  target_link_libraries(asmkit_core_tests PRIVATE asmkit)
  target_include_directories(asmkit_core_tests PRIVATE include tests)
  set_target_properties(asmkit_core_tests PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
  if(MSVC)
    target_compile_options(asmkit_core_tests PRIVATE /bigobj)
  endif()
  add_test(NAME asmkit_core_tests COMMAND asmkit_core_tests)

  add_test(NAME asmkit_runtime_forbidden_scan
    COMMAND ${CMAKE_COMMAND}
      -DROOT=${CMAKE_CURRENT_SOURCE_DIR}
      -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/allocation/scan_forbidden.cmake)
endif()
