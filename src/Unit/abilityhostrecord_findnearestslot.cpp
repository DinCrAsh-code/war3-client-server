//============================================================================
//  0x6F2DF170 - FindNearestReachableIndex.  Free function, not a member -
//  `ecx` is never read anywhere in the body.  Walks
//  g_indexedRecordArray_6FAB4E90's g_indexedRecordCount_6FAB4E74 entries
//  (indexedrecordarray.h), resolves each one's own +0x54 handle-ref pair,
//  and for every resolved object ranks it through the out-of-scope
//  0x6F4758D0 - `this` is that object's own +0x164 embedded path record
//  (CPathRef, the same "cast GetHandleObject() to CPathRef" shape
//  abilityhostrecord_positionnotify.cpp already uses) - keeping the index
//  of whichever entry ranked lowest.  `candidate` selects which of the two
//  argument shapes 0x6F4758D0 gets asked with: a real CWorldVec3 (read off
//  `candidate`'s own +0x164 path record) and its +0x34 rawcode when set,
//  or `fallback` verbatim and zeros otherwise.  Returns the winning index,
//  or -1 if nothing in range resolved.
//
//  0x6F4758D0 itself is out of this agent's own worklist slice - assigned
//  to a different follow-up agent (createunit-H3, reconstructing
//  0x6F2DEC20 in the same neighbourhood) - so every argument here past the
//  ones this call site's own disassembly pins down is a raw, address-
//  derived guess, not a claim about what 0x6F4758D0 does with them.
//============================================================================
#include "indexedrecordarray.h"
#include "game.h"
#include "itemhandleresolve.h"

//  0x6F4758D0 (`retn 20h`, 8 stack arguments) - see file header.
struct SUnknownRankFn_6F4758D0
{
    unsigned int Rank(const void* posOrFallback, unsigned int budget,
                       void* pathRefPtr, unsigned int otherField34,
                       void* other, unsigned int rawcode, int a, int b);
};
__declspec(naked) unsigned int SUnknownRankFn_6F4758D0::Rank(
    const void*, unsigned int, void*, unsigned int, void*, unsigned int, int, int)
{
    __asm { mov eax, 06F4758D0h }
    __asm { jmp eax }
}

int __stdcall FindNearestReachableIndex(void* candidate, const void* fallback)
{
    int bestIndex = -1;
    unsigned int bestValue = 0x50;
    unsigned int budget = 0x96;

    for (int i = 0; i < g_indexedRecordCount_6FAB4E74; i++)
    {
        void* entry = g_indexedRecordArray_6FAB4E90[i];
        SOptionalHandleRef* ref = (SOptionalHandleRef*)((char*)entry + 0x54);
        if (((unsigned int)ref->m_typeTag & ref->m_handle) == 0xFFFFFFFFu)
            continue;

        void* resolved = QueryHandleField0x54(ref);
        if (resolved == 0)
            continue;

        void* pathRefPtr = (char*)resolved + 0x164;
        unsigned int rank;

        if (candidate != 0)
        {
            void* other = (char*)candidate + 0x164;
            CWorldVec3 tmp = ((CPathRef*)other)->SmartPtrToWorldVec3();
            unsigned int rawcode = *(unsigned int*)((char*)candidate + 0x34);
            unsigned int* otherField34 = (unsigned int*)((char*)resolved + 0x34);
            rank = ((SUnknownRankFn_6F4758D0*)pathRefPtr)->Rank(
                &tmp, budget, pathRefPtr, *otherField34, other, rawcode, 1, 0);
        }
        else
        {
            unsigned int* otherField34 = (unsigned int*)((char*)resolved + 0x34);
            rank = ((SUnknownRankFn_6F4758D0*)pathRefPtr)->Rank(
                fallback, budget, pathRefPtr, *otherField34, 0, 0, 1, 0);
        }

        //  CPathSearch's own +0x9C is not otherwise named in pathfind.h -
        //  not reached by any target that class's own header already covers.
        unsigned int distance = *(unsigned int*)((char*)g_pTimeSync->GetPathOwner() + 0x9C) + 0x32;
        if (distance < budget)
            budget = distance;

        if (rank < bestValue)
        {
            bestIndex = i;
            bestValue = rank;
        }
    }

    return bestIndex;
}
