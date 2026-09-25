//============================================================================
//  0x6F4BC890 - reached from GameMainInitSequence
//  (Game/gamemaininitsequence.cpp): timestamp the run, resolve the
//  directory to scan, and enumerate "*.pre" files against a callback -
//  read as a stale ".pre" temp-file sweep, though nothing in this call
//  tree confirms that name.
//============================================================================
#include "os.h"

//  0x6F6C4F40/0x6F6C5060 - out of this session's scope (gamemain-batch-2
//  owns both).  GetSystemTime/SystemTimeToFileTime-shaped wrappers, per
//  their call site's own comments (`; lpSystemTime`, `; lpFileTime`).
//  Thunked.
__declspec(naked) void __fastcall OsGetSystemTime_6F6C4F40(void* /*lpSystemTime*/)
{
    __asm { mov eax, 0x6F6C4F40 }
    __asm { jmp eax }
}
__declspec(naked) void __fastcall OsSystemTimeToFileTime_6F6C5060(void* /*lpSystemTime*/, void* /*lpFileTime*/)
{
    __asm { mov eax, 0x6F6C5060 }
    __asm { jmp eax }
}

//  0x6F4BC740 - out of this session's scope (gamemain-batch-2 owns it); the
//  whole-program eax/ebx custom calling convention
//  (docs/msvc-vc8-idioms.md, "Arguments in eax and esi") - `eax` (a size)
//  and `ebx` (a buffer pointer) are both loaded immediately before the
//  call and never set by the callee.  No __fastcall spelling reaches
//  either register, so this is declared as an ordinary
//  __fastcall(buffer, size) instead (the ecx/edx pair CountLeadingZeros'
//  own idiom entry recommends reaching for); no stack argument either
//  side either way (`retn 0`), so the substitution costs this call site a
//  couple of register-choice instructions and nothing else - a
//  directory-path helper by every sign (a 0x104-byte MAX_PATH buffer, its
//  result compared against 0x103).  Thunked.
__declspec(naked) unsigned int __fastcall GetSomeDirectoryPath_6F4BC740(void* /*buffer*/, unsigned int /*size*/)
{
    __asm { mov eax, 0x6F4BC740 }
    __asm { jmp eax }
}

//  0x6F4BC800 - the enumeration callback, never called directly in this
//  tree (only its address is taken) and with no dump body at all - beyond
//  this session's depth-5 walk.  Thunked with OsEnumerateFiles's own
//  callback shape.
__declspec(naked) int __fastcall PreFileScanCallback_6F4BC800(OSFINDDATA*, void*)
{
    __asm { mov eax, 0x6F4BC800 }
    __asm { jmp eax }
}

void __fastcall CleanupPreFiles()
{
    char systemTime[0x10];   // SYSTEMTIME, opaque to this call tree
    __int64 fileTime;        // FILETIME

    OsGetSystemTime_6F6C4F40(systemTime);
    OsSystemTimeToFileTime_6F6C5060(systemTime, &fileTime);

    char directory[0x104];
    unsigned int length = GetSomeDirectoryPath_6F4BC740(directory, 0x104);
    if (length < 0x103)
    {
        directory[length] = '\\';
        directory[length + 1] = 0;
    }

    OsEnumerateFiles(directory, "*.pre", PreFileScanCallback_6F4BC800, &fileTime, 0, 1);
}
