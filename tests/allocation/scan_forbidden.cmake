if(NOT DEFINED ROOT)
  message(FATAL_ERROR "ROOT is required")
endif()

file(GLOB_RECURSE runtime_files
  "${ROOT}/include/*.h"
  "${ROOT}/src/*.h"
  "${ROOT}/src/*.c")

set(forbidden
  "malloc"
  "calloc"
  "realloc"
  "free"
  "pthread_"
  "SRWLOCK"
  "futex"
  "mutex"
  "semaphore"
  "fopen"
  "open\\("
  "printf"
  "snprintf")

foreach(file IN LISTS runtime_files)
  file(READ "${file}" content)
  foreach(pattern IN LISTS forbidden)
    if(content MATCHES "(^|[^A-Za-z0-9_])${pattern}([^A-Za-z0-9_]|$)")
      message(FATAL_ERROR "Forbidden runtime symbol '${pattern}' found in ${file}")
    endif()
  endforeach()
endforeach()
