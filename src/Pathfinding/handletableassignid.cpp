//============================================================================
//  0x6F4AEFC0/0x6F4AEF00 - SHandleTableEntry::AssignFreshHandleId and
//  ::UnregisterAndNotify.  See handletable.h for what g_pHandleTable is and
//  why it is not the checksum-provider registry's own "tempest presence"
//  host despite the identical field shape - including that host's own
//  now-confirmed RTTI identity (`NTempest::CDynTable<CPrRgEntry>`), which
//  handletable.h's own note is explicit does not carry over to
//  g_pHandleTable.
//
//  Kept together (rather than one file each): neither calls the other, so
//  there is no real cross-call for this build's /Ob2 to inline away by
//  having both bodies visible in one TU.  RegisterOrReuse
//  (handletableregister.cpp) is the one function in this small family with
//  a real call into AssignFreshHandleId, which is why *that* pair is split
//  across two files instead.
//============================================================================
#include "handletable.h"

void SHandleTableEntry::AssignFreshHandleId()
{
    if (m_slotHandle & 0x80000000u)
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

void SHandleTableEntry::UnregisterAndNotify(int /*arg*/)
{
    if (m_slotHandle != 0xFFFFFFFFu)
        g_pHandleTable->Push((int)m_slotHandle, (int)this);

    typedef void (__thiscall *NotifyFn)(void*);
    NotifyFn notify = (NotifyFn)(*(void***)this)[1];

    m_slotIndex = 0xFFFFFFFFu;
    m_slotHandle = 0xFFFFFFFFu;

    notify(this);
}
