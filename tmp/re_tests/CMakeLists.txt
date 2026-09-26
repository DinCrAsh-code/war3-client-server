cmake_minimum_required(VERSION 3.10)
project(re_tests CXX)

# Builds every 0N_* test project in this repo with one command:
#
#   cmake -S . -B build [-G "NMake Makefiles" -DCMAKE_TOOLCHAIN_FILE=cmake/VS8Toolchain.cmake] -DCMAKE_BUILD_TYPE=Release
#   cmake --build build
#
# All projects are always built as optimized, release binaries with no
# debug info and symbols stripped, regardless of CMAKE_BUILD_TYPE, since
# the whole point of this repo is producing realistic RE targets.

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

if(MSVC)
    # Strip any /Zi /ZI /Z7 that CMake's default flag sets may have added,
    # so no PDB / debug info gets generated even if someone passes
    # CMAKE_BUILD_TYPE=Debug or RelWithDebInfo.
    foreach(flag_var
            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_MINSIZEREL
            CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_C_FLAGS_MINSIZEREL)
        string(REGEX REPLACE "/Z[iI7]" "" ${flag_var} "${${flag_var}}")
        set(${flag_var} "${${flag_var}}" CACHE STRING "" FORCE)
    endforeach()

    add_compile_options(/O2 /GL /DNDEBUG /EHsc)
    add_link_options(/RELEASE /OPT:REF /OPT:ICF /LTCG /INCREMENTAL:NO)
else()
    # Non-MSVC fallback (useful for a quick sanity build on Linux/mingw
    # while iterating, even though the real target is VS8/Windows).
    add_compile_options(-O2 -DNDEBUG -fno-ident -fvisibility=hidden)
    add_link_options(-s)
endif()

add_subdirectory(01_HelloWorldExample)
add_subdirectory(02_MoreAdvancedExample)
add_subdirectory(03_ClassesAndVirtualsExample)
add_subdirectory(04_MiniRtsExample)
