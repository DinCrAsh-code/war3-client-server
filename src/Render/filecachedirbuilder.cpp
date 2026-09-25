//============================================================================
//  0x6F4BC740 - append "FileCache" onto the process's own module directory
//  (OsGetModuleDirectory, Os/osmoduledirectory.cpp) already written into the
//  caller's buffer, and hand back the combined length.
//
//  Not reachable through any MSVC calling-convention keyword: the two
//  arguments arrive in `eax` (buffer size) and `ebx` (buffer pointer)
//  rather than any register `__fastcall`/`__thiscall` ever use, which means
//  this is a shared tail some other, not-yet-reconstructed function in this
//  neighbourhood `jmp`s into with those two registers already loaded (the
//  same "no caller in this call tree, args in registers no spelling
//  reaches" shape docs/msvc-vc8-idioms.md and CLAUDE.md's own
//  `tools/unhookable.py` notes document elsewhere) - so it is transcribed
//  naked rather than guessed at as an ordinary function.  A naked body is
//  not eligible for the register/scheduling excuses CLAUDE.md's own
//  BEHAVIOUR table lists, so this should score 1.000 by construction.
//============================================================================
#include "os.h"     // OsGetModuleDirectory - re-declaring it locally with a
                     // slightly different parameter type (DWORD vs unsigned
                     // int) mangles to a different symbol and only fails at
                     // link time; including the real header avoids that.
unsigned int __stdcall Storm_506(const char* s);
int Storm_578(char* dest, unsigned int size, const char* format, ...);

extern "C" const char szFileCache[] = "FileCache";
extern "C" const char szPercentS[]  = "%s";

__declspec(naked) unsigned int FileCacheDirBuilder()
{
    __asm {
        push esi
        mov esi, eax
        push edi
        mov edx, esi
        mov ecx, ebx
        call OsGetModuleDirectory
        push ebx
        call Storm_506
        mov edi, eax
        push offset szFileCache
        push offset szPercentS
        sub esi, edi
        lea eax, [edi + ebx]
        push esi
        push eax
        call Storm_578
        add esp, 10h
        add eax, edi
        pop edi
        pop esi
        retn
    }
}
