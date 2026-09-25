//============================================================================
//  0x6F2E1060 - FindOrCreateIndexedRecord.  Free function (`ecx` is only
//  ever a copy of `candidate`, never dereferenced as a receiver of its
//  own), `retn 8`.  Ask the out-of-scope 0x6F2DEC20 (assigned to a
//  different follow-up agent, createunit-H3, alongside its own
//  0x6F2DEC20 worklist entry) for an index into
//  g_indexedRecordArray_6FAB4E90; if it found one (>= 0), return that
//  entry verbatim.  Otherwise construct a fresh, empty
//  SCheckedSlot_6F2D04D0 (checkedslot_6f2d04d0.h, already reconstructed)
//  and return whatever it resolved to - the temporary's own destructor
//  releases the extra reference before returning, the same
//  construct-read-release shape GetUnitIfResolvedAt0x2C4
//  (abilityhostrecord_handlerefs.cpp) already uses.
//============================================================================
#include "indexedrecordarray.h"
#include "checkedslot_6f2d04d0.h"

//  0x6F2DEC20 - out of scope (createunit-H3).  `this` and the explicit
//  first stack argument are both `candidate` at this call site - a raw,
//  address-derived guess past what the call site itself pins down.
struct SUnknownFinder_6F2DEC20
{
    int FindOrMakeIndex(void* self, int flag, void* extra);
};
__declspec(naked) int SUnknownFinder_6F2DEC20::FindOrMakeIndex(void*, int, void*)
{
    __asm { mov eax, 06F2DEC20h }
    __asm { jmp eax }
}

void* __stdcall FindOrCreateIndexedRecord(void* candidate, void* extra)
{
    int index = ((SUnknownFinder_6F2DEC20*)candidate)->FindOrMakeIndex(candidate, 1, extra);
    if (index >= 0)
    {
        return g_indexedRecordArray_6FAB4E90[index];
    }

    SCheckedSlot_6F2D04D0 slot(0);
    return slot.m_value;
}
