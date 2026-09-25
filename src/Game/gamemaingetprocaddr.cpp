//============================================================================
//  0x6F00BA90 - GameMainSetupWorldSecurityDescriptor's (0x6F00BAB0,
//  gamemainsecuritydescriptor.cpp) own GetProcAddress helper.
//
//  A whole-program-codegen custom calling convention (docs/msvc-vc8-idioms.md,
//  "Arguments in eax and esi") - every one of this tree's four call sites
//  loads the proc name into `eax` and the module handle into `ecx`
//  immediately before the call, and the callee never sets either register
//  itself.  No MSVC __fastcall spelling reaches `eax`; declared with the
//  procedure name in `edx` instead (the second free __fastcall register) so
//  the one real stack argument (the out-pointer, `retn 4`) still lines up
//  exactly - the only difference from the shipped call sites is which
//  register carries the name, `IDENTICAL`.
//============================================================================
#include <windows.h>

int __fastcall ResolveModuleProc(HMODULE hModule, const char* procName, void** out)
{
    void* proc = (void*)GetProcAddress(hModule, procName);
    if (!proc)
        return 0;

    *out = proc;
    return 1;
}
