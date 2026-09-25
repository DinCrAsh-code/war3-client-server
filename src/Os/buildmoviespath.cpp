//============================================================================
//  0x6F5BC8A0 - BuildMoviesPathB3 (gamemain-batch-3).  Format
//  "<module directory>Movies\<name>.mpq" into the caller's buffer:
//  OsGetModuleDirectory (0x6F6C6000, Os/osmoduledirectory.cpp) supplies the
//  module directory, Storm_578 (SStrPrintf-family) does the formatting.
//  What is NOT reproduced: the /GS stack-buffer cookie this frame's own
//  0x104-byte local carries - accepted gap, same as osversionclass.cpp.
//============================================================================
#include "os.h"

int Storm_578(char* dest, unsigned int size, const char* format, ...);

void __fastcall BuildMoviesPathB3(const char* name, char* destBuffer,
                                  unsigned int destSize)
{
    char moduleDir[0x104];
    OsGetModuleDirectory(moduleDir, 0x104);

    Storm_578(destBuffer, destSize, "%sMovies\\%s.mpq", moduleDir, name);
}
