# CMake toolchain file for building with an out-of-tree VS8 (Visual Studio
# 2005 / VC80) installation that is NOT registered in the Windows registry
# (e.g. a copy of the VS8 install tree kept in its own folder, separate
# from whatever compiler is "installed" on the machine).
#
# Usage (from a Windows shell, with a real VS8 install pointed to by
# paths.cmake):
#
#   cmake -S . -B build -G "NMake Makefiles" ^
#         -DCMAKE_TOOLCHAIN_FILE=cmake/VS8Toolchain.cmake ^
#         -DCMAKE_BUILD_TYPE=Release
#   build.bat
#
# Requires paths.cmake in the repo root (copy paths.cmake.example ->
# paths.cmake and edit VS8_ROOT for your machine).
#
# NMake Makefiles is used instead of a "Visual Studio 8 2005" generator
# because that generator relies on VS8 being registered with the system;
# driving cl.exe/link.exe directly works for an arbitrary install path.
#
# IMPORTANT: use build.bat (repo root) to actually build, not a bare
# `cmake --build build`. cl.exe/link.exe/lib.exe need mspdb80.dll (from
# Common7/IDE) on PATH and the standard headers/libs on INCLUDE/LIB. Those
# get set on ENV{} below for THIS configure-time process only - a plain
# `cmake --build` starts a brand new process (nmake) from your shell that
# never inherits them, and cl.exe fails with STATUS_DLL_NOT_FOUND
# (0xc0000135). build.bat sources the vs8-env.cmd generated below into its
# own single shell session before invoking `cmake --build`, so the whole
# nmake/cl/link process tree inherits the right environment.

get_filename_component(_RE_TESTS_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(NOT EXISTS "${_RE_TESTS_ROOT}/paths.cmake")
    message(FATAL_ERROR
        "paths.cmake not found in repo root. Copy paths.cmake.example to "
        "paths.cmake and set VS8_ROOT to your VS8 installation path.")
endif()

include("${_RE_TESTS_ROOT}/paths.cmake")

if(NOT VS8_ROOT)
    message(FATAL_ERROR "VS8_ROOT is not set. Edit paths.cmake.")
endif()

set(VS8_VC_DIR  "${VS8_ROOT}/VC")
set(VS8_IDE_DIR "${VS8_ROOT}/Common7/IDE")

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86)

set(CMAKE_C_COMPILER   "${VS8_VC_DIR}/bin/cl.exe")
set(CMAKE_CXX_COMPILER "${VS8_VC_DIR}/bin/cl.exe")
set(CMAKE_LINKER       "${VS8_VC_DIR}/bin/link.exe")
set(CMAKE_AR           "${VS8_VC_DIR}/bin/lib.exe")
set(CMAKE_RC_COMPILER  "${VS8_VC_DIR}/bin/rc.exe")

# The "NMake Makefiles" generator normally locates nmake.exe by detecting
# a registered Visual Studio install; since VS8_ROOT isn't registered,
# that detection finds nothing and CMAKE_MAKE_PROGRAM is left unset/bare
# ("nmake"), which fails at build time ("no such file or directory").
# Point it at VS8's own nmake.exe explicitly.
set(CMAKE_MAKE_PROGRAM "${VS8_VC_DIR}/bin/nmake.exe" CACHE FILEPATH "Path to nmake" FORCE)

# NOTE: VC80's own include/lib do NOT contain the Win32 API (kernel32.lib,
# windows.h, ...) - that comes from a separate Platform SDK. A VS2005
# install normally registers one system-wide, but a standalone/copied VS8
# tree often bundles its own subset under VC/PlatformSDK. If WINSDK_ROOT
# isn't set in paths.cmake, fall back to that bundled copy when present.
set(_VS8_INCLUDE "${VS8_VC_DIR}/include")
set(_VS8_LIB "${VS8_VC_DIR}/lib")

if(NOT WINSDK_ROOT AND EXISTS "${VS8_VC_DIR}/PlatformSDK/Lib")
    set(WINSDK_ROOT "${VS8_VC_DIR}/PlatformSDK")
endif()

if(WINSDK_ROOT)
    set(_VS8_INCLUDE "${_VS8_INCLUDE};${WINSDK_ROOT}/Include")
    set(_VS8_LIB "${_VS8_LIB};${WINSDK_ROOT}/Lib")
else()
    message(WARNING
        "No Windows SDK found (WINSDK_ROOT unset and no ${VS8_VC_DIR}/PlatformSDK). "
        "Linking will fail with 'cannot open file kernel32.lib' unless you "
        "set WINSDK_ROOT in paths.cmake to a Platform SDK containing "
        "Include/ and Lib/ with kernel32.lib etc.")
endif()
if(EXTRA_INCLUDE_DIRS)
    set(_VS8_INCLUDE "${_VS8_INCLUDE};${EXTRA_INCLUDE_DIRS}")
endif()
if(EXTRA_LIB_DIRS)
    set(_VS8_LIB "${_VS8_LIB};${EXTRA_LIB_DIRS}")
endif()

# Set for configure-time compiler/ABI checks (try_compile spawns as a
# child of this cmake process and inherits these).
set(ENV{PATH}    "${VS8_IDE_DIR};${VS8_VC_DIR}/bin;$ENV{PATH}")
set(ENV{INCLUDE} "${_VS8_INCLUDE}")
set(ENV{LIB}     "${_VS8_LIB}")

# Also write out a standalone env script with the same PATH/INCLUDE/LIB,
# for build.bat to `call` before `cmake --build`, so the build-time
# process tree (nmake -> cl/link/lib) gets it too.
file(WRITE "${CMAKE_BINARY_DIR}/vs8-env.cmd"
"@echo off\r\n\
set \"PATH=${VS8_IDE_DIR};${VS8_VC_DIR}/bin;%PATH%\"\r\n\
set \"INCLUDE=${_VS8_INCLUDE}\"\r\n\
set \"LIB=${_VS8_LIB}\"\r\n")

set(CMAKE_TRY_COMPILE_PLATFORM_VARIABLES VS8_ROOT WINSDK_ROOT)
