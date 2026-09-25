//============================================================================
//  0x6F48F0A0/0x6F48EE90 - two small leaves NIpse::CPrClusterGen::Apply/
//  Slot4 (prclustergen_ctordtor.cpp) each call once, for real, so both stay
//  out of that file's own translation unit.
//============================================================================
#include "prcluster.h"

namespace NIpse {

//  0x6F48F0A0 - reset every one of CPrClusterGen's own twelve
//  `m_refSlots`/`m_resolved` pairs to their "empty" sentinel: each
//  `SPrClusterGenRefSlot`'s own `m_handle`/`m_typeTag` to -1 (its own
//  `m_field08` is NOT touched - a real asymmetry confirmed off the dump)
//  and the parallel `m_resolved[i]` to `(CHandleObject*)-1`.  `this` in
//  `ecx`, `retn` with zero stack bytes.
void CPrClusterGen::ResetRefSlots()
{
    for (unsigned int i = 0; i < 12; i++)
    {
        m_refSlots[i].m_typeTag = (unsigned int)-1;
        m_refSlots[i].m_handle = (unsigned int)-1;
        m_resolved[i] = (CHandleObject*)-1;
    }
}

//  0x6F48EE90 - assign a refcounted pointer into m_field0F8: decrement the
//  OLD pointer's own +0x1C refcount field (if non-null), increment the NEW
//  one's (if non-null), then store it.  `this` in `ecx`, one stack argument
//  (the new pointer), `retn 4`.
void CPrClusterGen::AssignField0F8(void* value)
{
    void* old = m_field0F8;
    if (old != 0)
        *(int*)((char*)old + 0x1C) -= 1;
    if (value != 0)
        *(int*)((char*)value + 0x1C) += 1;
    m_field0F8 = value;
}

}  // namespace NIpse
