cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(robotics_benchmarks)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall")

# Default to Release build
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

# By default, build statically for maximum performance.
if(NOT BUILD_DYNAMIC)
  set(BUILD_SHARED_LIBS OFF)
  set(CMAKE_EXE_LINKER_FLAGS
      "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
  # only find static libraries (for librt needed by benchmark).
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

  # Some additional magic needed to properly link statically when using pthread.
  # https://stackoverflow.com/a/45271521
  set(
    CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,--whole-archive -lpthread -Wl,--no-whole-archive"
    )
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx")

set(BENCHMARK_ENABLE_TESTING OFF
    CACHE BOOL "Suppressing benchmark's tests"
    FORCE)
add_subdirectory(vendor/benchmark)

add_executable(bm src/kalman.cc)
target_compile_options(bm PRIVATE -Wall -Werror)
target_include_directories(bm SYSTEM PUBLIC vendor/eigen vendor vendor/Fastor)
target_link_libraries(bm benchmark)
