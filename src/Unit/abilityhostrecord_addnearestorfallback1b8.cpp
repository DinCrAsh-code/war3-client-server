//============================================================================
//  0x6F2E5060 - SAbilityHostRecord::AddNearestOrFallbackTo1B8Set
//  (abilityhostrecord.h).  The general form AddNearestTo1B8Set
//  (abilityhostrecord_addnearest1b8.cpp) is the `fallback == 0` case of:
//  find the nearest reachable index; if one resolved, notify that record's
//  own indexed-array entry through the out-of-scope 0x6F2E3790 (only when
//  `candidate` is set) and add the index; if none resolved, run one of two
//  out-of-scope side effects instead (0x6F2E4FC0 when `candidate` is set,
//  0x6F2E4F00(fallback, fallback+4) when it is not) and add the *current*
//  g_indexedRecordCount_6FAB4E74 (a fresh, about-to-be-appended slot,
//  presumably - the append itself is inside whichever of those two
//  out-of-scope calls ran).
//============================================================================
#include "abilityhostrecord.h"
#include "indexedrecordarray.h"

//  0x6F2DF170 - FindNearestReachableIndex, already reconstructed
//  (abilityhostrecord_findnearestslot.cpp).
int __stdcall FindNearestReachableIndex(void* candidate, const void* fallback);

//  0x6F2E3790 - SHandleRefHost6F2E3790::UpdateRef48, already reconstructed
//  (Misc/cunit_agent8_batch3.cpp); re-declared here so the mangled name
//  binds to that one real symbol.
struct SHandleRefHost6F2E3790
{
    void UpdateRef48(void* newTarget);
};

//  0x6F2E4FC0 - out of scope, not otherwise identified.  `this` is the
//  SAbilityHostRecord; `retn 4`, one stack argument (always 0 here).
struct SUnknownAppend_6F2E4FC0
{
    void Append(int flag);
};
__declspec(naked) void SUnknownAppend_6F2E4FC0::Append(int)
{
    __asm { mov eax, 06F2E4FC0h }
    __asm { jmp eax }
}

//  0x6F2E4F00 - out of scope, not otherwise identified.  `this` is the
//  SAbilityHostRecord; `retn 8`, two stack arguments (`fallback`,
//  `fallback + 4`).
struct SUnknownAppend_6F2E4F00
{
    void Append(const void* fallback, const void* fallbackPlus4);
};
__declspec(naked) void SUnknownAppend_6F2E4F00::Append(const void*, const void*)
{
    __asm { mov eax, 06F2E4F00h }
    __asm { jmp eax }
}

void SAbilityHostRecord::AddNearestOrFallbackTo1B8Set(void* candidate, const void* fallback)
{
    int index = FindNearestReachableIndex(candidate, fallback);
    if (index == -1)
    {
        index = g_indexedRecordCount_6FAB4E74;
        if (candidate == 0)
            ((SUnknownAppend_6F2E4F00*)this)->Append(fallback, (const char*)fallback + 4);
        else
            ((SUnknownAppend_6F2E4FC0*)this)->Append(0);
    }
    else if (candidate != 0)
    {
        void* entry = g_indexedRecordArray_6FAB4E90[index];
        ((SHandleRefHost6F2E3790*)entry)->UpdateRef48(candidate);
    }

    AddUniqueTo1B8Set((unsigned int)index);
}
