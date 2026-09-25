//============================================================================
//  0x6F4AEFF0 - SHandleTableEntry::RegisterOrReuse.  See handletable.h -
//  including its note on why g_pHandleTable is unaffected by the tempest
//  presence host's now-confirmed `NTempest::CDynTable<CPrRgEntry>` identity.
//
//  `request` (or its absence, or a `m_priorSlot` of -1) means "just mint a
//  brand new slot"; `m_priorSlot == -2` means "mint one, but always from
//  the negative pool"; any other `m_priorSlot` means "reuse this exact
//  already-known slot" (re-minting a fresh id only when `m_secondSlot` is
//  still -1 - when both are already known, the two fields are simply set
//  equal to the same handle STwoIndexedFreeLists::AllocateAt just
//  returned, with no fresh id minted at all).
//
//  Own translation unit: the shipped body reaches AssignFreshHandleId
//  (0x6F4AEFC0) with a real `call` from two of its three tail paths - the
//  "fresh slot" path's own tail, unlike the other two, inlines
//  AssignFreshHandleId's own two-branch body instead of calling it (read
//  directly off the dump: `jns`/`[ecx+50h]`/`[ecx+54h]` right here, no
//  `call sub_6F4AEFC0` on that path) - transcribed as-is rather than
//  "cleaned up" into a third call, since the shipped instruction stream is
//  what a match is judged against, not what looks more uniform.
//============================================================================
#include "handletable.h"

void SHandleTableEntry::RegisterOrReuse(SHandleRegistrationRequest* request)
{
    if (request && request->m_priorSlot != -1)
    {
        if (request->m_priorSlot == -2)
        {
            m_slotHandle = g_pHandleTable->AllocateAt(this, 1, -1);
            AssignFreshHandleId();
            return;
        }

        m_slotHandle = g_pHandleTable->AllocateAt(this, 0, request->m_priorSlot);

        int secondSlot = request->m_secondSlot;
        if (secondSlot == -1)
        {
            AssignFreshHandleId();
            return;
        }

        m_slotIndex = m_slotHandle;
        return;
    }

    unsigned int handle = g_pHandleTable->AllocateAt(this, 0, -1);
    m_slotHandle = handle;

    if (handle & 0x80000000u)
    {
        int id = g_pHandleTable->m_nextNegativeId;
        g_pHandleTable->m_nextNegativeId = id + 1;
        m_slotIndex = (unsigned int)id;
    }
    else
    {
        int id = g_pHandleTable->m_nextPositiveId;
        g_pHandleTable->m_nextPositiveId = id + 1;
        m_slotIndex = (unsigned int)id;
    }
}
