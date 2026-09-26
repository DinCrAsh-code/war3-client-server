@echo off
setlocal

set "BUILD_DIR=%~1"
if "%BUILD_DIR%"=="" set "BUILD_DIR=build"

if not exist "%BUILD_DIR%\vs8-env.cmd" (
    echo.
    echo %BUILD_DIR%\vs8-env.cmd not found. Configure first, e.g.:
    echo   cmake -S . -B %BUILD_DIR% -G "NMake Makefiles" -DCMAKE_TOOLCHAIN_FILE=cmake/VS8Toolchain.cmake -DCMAKE_BUILD_TYPE=Release
    echo.
    exit /b 1
)

rem Load PATH/INCLUDE/LIB for VS8 (mspdb80.dll, headers, libs) into THIS
rem shell session, then build in the same session, so cl.exe/link.exe/
rem lib.exe (spawned by nmake) inherit it. A bare `cmake --build` run
rem directly from your own shell won't have this environment set.
call "%BUILD_DIR%\vs8-env.cmd"

cmake --build "%BUILD_DIR%"
exit /b %ERRORLEVEL%
