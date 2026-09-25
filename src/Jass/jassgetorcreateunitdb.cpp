//============================================================================
//  0x6F71BF00 - CUnitDB's own lazy singleton getter (the SEH-cookie-framed
//  double-checked init `__except_handler4`-shaped prologue
//  docs/msvc-vc8-idioms.md's own "An `__except_handler4`-shaped frame this
//  toolchain cannot reproduce" section documents; this repo's fixed
//  `/GS- /EHs-c-` build cannot emit that exact frame from any spelling of
//  a `__try`/`__finally`, so the gap below is that, not a logic mismatch).
//  Allocates 0x1F0 bytes tagged with WorldEdit/WEUtilities.cpp's own
//  `__FILE__`/line (0xD5C), constructs `CUnitDB` into it via
//  sub_6F70FC90 (out of this session's own closure - see
//  jassloadsheetfield.cpp's own note on that function's four
//  `LoadSheetFieldAndArm` calls over "Units\\UnitData" and its siblings,
//  which is what identifies the constructed class), and caches the result
//  in `dword_6FADB0C8`.
//============================================================================
#include "storm.h"   // SMemAlloc

//  0x6F70FC90 - `retn 0`, plain `__thiscall` (`ecx` = the allocated
//  block, returned unchanged in `eax` - the shipped call site immediately
//  stores eax into dword_6FADB0C8, confirming the constructor's own
//  `this`-in-eax return is read directly rather than discarded). Same
//  real address as CUnitDB::CUnitDB() (src/Misc/jscriptbin4_thunks.cpp,
//  out of this session's own closure - see jassloadsheetfield.cpp's own
//  note on the four `LoadSheetFieldAndArm` calls that identify the
//  constructed class) - a second naked thunk to the same address under
//  this file's own call-site signature, the same "one shipped address,
//  two declared names" shape RegisterNative's own note documents
//  elsewhere in src/Jass/, needed because a real C++ constructor call
//  expression doesn't let the assignment read eax the way this manually
//  declared `void*`-returning method does.
struct SUnitDBCtorTarget
{
    void* Construct();
};

__declspec(naked) void* SUnitDBCtorTarget::Construct()
{
    __asm { mov eax, 06F70FC90h }
    __asm { jmp eax }
}

extern void* g_unitDBSingleton;   // dword_6FADB0C8

void* __cdecl GetOrCreateUnitDB()
{
    if (g_unitDBSingleton == 0)
    {
        void* mem = SMemAlloc(0x1F0,
            "e:\\Drive1\\temp\\buildwar3x\\War3\\Source\\WorldEdit/WEUtilities.cpp",
            0xD5C, 0);

        if (mem != 0)
            g_unitDBSingleton = ((SUnitDBCtorTarget*)mem)->Construct();
        else
            g_unitDBSingleton = 0;
    }

    return g_unitDBSingleton;
}
