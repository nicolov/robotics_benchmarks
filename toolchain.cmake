# Read target triple and ct-ng toolchain location from the env vars
set(ct_ng_triple "$ENV{TRIPLE}")
if (DEFINED ENV{CT_NG_ROOT_DIR})
    set(ct_ng_root_dir "$ENV{CT_NG_ROOT_DIR}")
else ()
    set(ct_ng_root_dir "/opt/x-tools")
endif ()

set(ct_ng_home "${ct_ng_root_dir}/${ct_ng_triple}")
set(CMAKE_PROGRAM_PATH "${ct_ng_home}/bin")
set(CMAKE_SYSROOT "${ct_ng_home}/${ct_ng_triple}/sysroot")

set(CMAKE_LIBRARY_ARCHITECTURE ${ct_ng_triple})
set(CMAKE_C_COMPILER_TARGET ${ct_ng_triple})
set(CMAKE_CXX_COMPILER_TARGET ${ct_ng_triple})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

if ("$ENV{USE_CLANG}" STREQUAL "1")
    # https://cmake.org/pipermail/cmake-developers/2013-June/019171.html
    if (DEFINED ENV{CLANG_ROOT})
        set(clang_root "$ENV{CLANG_ROOT}")
    else ()
        set(clang_root "/opt/clang")
    endif ()

    set(CMAKE_C_COMPILER "${clang_root}/bin/clang")
    set(CMAKE_CXX_COMPILER "${clang_root}/bin/clang++")

    # We need to force this in the cache so that CMake will apply the flags when
    # building its compiler detection scripts. See
    # https://cmake.org/pipermail/cmake/2012-January/048429.html
    set(CMAKE_EXE_LINKER_FLAGS "--gcc-toolchain=${ct_ng_home}" CACHE STRING "--gcc-toolchain=${ct_ng_home}")

    include_directories("${ct_ng_home}/${ct_ng_triple}/include/c++/8.1.0" SYSTEM)
    include_directories("${ct_ng_home}/${ct_ng_triple}/include/c++/8.1.0/${ct_ng_triple}" SYSTEM)
else ()
    # Use gcc from the ct-ng toolchain.
    set(CMAKE_C_COMPILER "${ct_ng_home}/bin/${ct_ng_triple}-gcc")
    set(CMAKE_CXX_COMPILER "${ct_ng_home}/bin/${ct_ng_triple}-g++")
endif ()
