//============================================================================
//  0x6F0094F0 - read five preferences (ids 0,1,2,4,3) out of the singleton
//  and forward all five plus this function's own stack argument into
//  0x6F009190.
//============================================================================
#include "preferences.h"

//  0x6F009190 - below this batch's own 43; declared and thunked to its own
//  real address.  Thiscall plus one extra register argument (edx) and six
//  stack arguments - `retn 18h` confirmed by the six pushes at this call
//  tree's one call site.
__declspec(naked) void __fastcall ApplyFivePreferences(
    void* /*ecx*/, int /*edx*/, int, int, int, int, int, int)
{
    __asm { mov eax, 06F009190h }
    __asm { jmp eax }
}

void __fastcall ApplyStartupPreferences(void* self, int edxArg, int stackArg)
{
    int v0, v1, v2, v3, v4;
    GetPreferences()->GetIntValue(0, &v0);
    GetPreferences()->GetIntValue(1, &v1);
    GetPreferences()->GetIntValue(2, &v2);
    GetPreferences()->GetIntValue(4, &v3);
    GetPreferences()->GetIntValue(3, &v4);

    ApplyFivePreferences(self, edxArg, stackArg, v0, v1, v2, v3, v4);
}
