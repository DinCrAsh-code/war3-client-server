//============================================================================
//  0x6F005760 - the CPreferencesWar3 singleton's own teardown, the mirror
//  of GetPreferences (0x6F005720, preferences.cpp).  Reached out of
//  GameMainInitSequence (Game/gamemaininitsequence.cpp).
//============================================================================
#include "preferences.h"

extern CPreferencesWar3* g_pPreferences;   // dword_6FAAE314

//  Storm ordinal 403 - already declared this shape elsewhere (SMemFree).
void __stdcall SMemFree(void* ptr, const char* logfilename, int logline, unsigned int flags);

//  0x6F003C90/0x6F003B90/0x6F0053B0 - out of this session's scope
//  (gamemain-batch-3 owns all three); the two hash-table teardowns take
//  their one flag on the stack, not in edx (a genuine thiscall member, not
//  a two-register __fastcall - no `mov edx,...` precedes either call, just
//  `push 0`).  Thunked.
struct TeardownHelper
{
    void DestroyCache_6F003C90(int flag);
    void DestroyTable_6F003B90(int flag);
    void Dtor_6F0053B0();
};

void TeardownHelper::DestroyCache_6F003C90(int flag)
{
    __asm { mov eax, 0x6F003C90 }
    __asm { jmp eax }
}
void TeardownHelper::DestroyTable_6F003B90(int flag)
{
    __asm { mov eax, 0x6F003B90 }
    __asm { jmp eax }
}
void TeardownHelper::Dtor_6F0053B0()
{
    __asm { mov eax, 0x6F0053B0 }
    __asm { jmp eax }
}

void __fastcall DestroyPreferences()
{
    if (g_pPreferences)
    {
        ((TeardownHelper*)&g_pPreferences->m_cache)->DestroyCache_6F003C90(0);
        ((TeardownHelper*)g_pPreferences)->DestroyTable_6F003B90(0);

        if (g_pPreferences)
        {
            ((TeardownHelper*)g_pPreferences)->Dtor_6F0053B0();
            SMemFree(g_pPreferences, "delete", -1, 0);
        }
    }

    g_pPreferences = 0;
}
