cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(robotics_benchmarks)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

# statically link everything
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")

# sltbench benchmarking library
add_library(sltbench
    vendor/sltbench/src/BenchApi.cpp
    vendor/sltbench/src/Config.cpp
    vendor/sltbench/src/Config.h
    vendor/sltbench/src/ConsoleReporter.cpp
    vendor/sltbench/src/ConsoleReporter.h
    vendor/sltbench/src/CsvReporter.cpp
    vendor/sltbench/src/CsvReporter.h
    vendor/sltbench/src/DoNotOptimize.cpp
    vendor/sltbench/src/Env.cpp
    vendor/sltbench/src/Filters.cpp
    vendor/sltbench/src/Filters.h
    vendor/sltbench/src/JsonReporter.cpp
    vendor/sltbench/src/JsonReporter.h
    vendor/sltbench/src/MAResultsContainer.cpp
    vendor/sltbench/src/MAResultsContainer.h
    vendor/sltbench/src/MeasureAlgo.cpp
    vendor/sltbench/src/MeasureAlgo.h
    vendor/sltbench/src/ProcedureBenchmark.cpp
    vendor/sltbench/src/ProcedureBenchmark.h
    vendor/sltbench/src/ProgramOptions.cpp
    vendor/sltbench/src/ProgramOptions.h
    vendor/sltbench/src/Runner.cpp
    vendor/sltbench/src/Runner.h
    vendor/sltbench/src/SingleMeasureAlgo.cpp
    vendor/sltbench/src/SysInfo.cpp
    vendor/sltbench/src/SysInfo.h
    vendor/sltbench/src/Verdict.cpp
    vendor/sltbench/src/WarningsMessages.h
    vendor/sltbench/src/WarningsMessages.cpp
)
target_include_directories(sltbench
    SYSTEM PUBLIC vendor/sltbench/include
)

add_executable(bm
    src/kalman.cc
)
target_compile_options(bm
    PRIVATE -Wall -Werror
)
target_include_directories(bm
    SYSTEM PUBLIC vendor vendor/eigen
)
target_link_libraries(bm
    sltbench)
