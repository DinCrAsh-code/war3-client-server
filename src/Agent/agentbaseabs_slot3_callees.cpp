//============================================================================
//  0x6F4A45E0 / 0x6F4A45B0 / 0x6F4A4AD0 - the three callees of
//  `NIpse::CRlAgent::Apply` (agentbaseabs_slot3.cpp - reached through, but
//  not owned by, CAgentBaseAbs's own vtable slot 3) and of CRelatedAgentsTable::
//  GrowInsert (agentbaseabs_related.h) that were forward-declared as
//  CAgentBaseAbsSlot3Arg::LookUp_6F4A45E0 / CAgentBaseAbsSlot3Result::
//  FixUp_6F4A45B0 / CRelatedAgentsTableInsertExternal::InsertAt_6F4A4AD0
//  while owned by a different parallel session on the same dump - this
//  fills in the real bodies under those exact already-forward-declared
//  names so no reconciliation is needed at either call site.
//
//  0x6F4A45E0 - a one-instruction field accessor, `this+0x0C`, thiscall,
//  no stack args (`retn` with no operand).
//
//  0x6F4A45B0 - the same doubly-linked `{prev @ +4, next @ +8}` splice
//  `SRelationLink::InsertAfter` (0x6F4A5BE0, already reconstructed by a
//  parallel session) uses, just for this list's own node shape: insert
//  `outerAgent` (the one stack arg) immediately after `this` (ecx) -
//  `outerAgent->prev = this`, `outerAgent->next = this->next`, fix up the
//  old next's own `->prev` if there was one, then `this->next = outerAgent`.
//
//  0x6F4A4AD0 - the bounded fill loop `CRelatedAgentsTable::GrowInsert`
//  (0x6F4A4FE0, a parallel session's own reconstruction) hands off to once
//  room has been confirmed: `items` is read through *once per iteration*
//  (`mov ebx,[esi]` inside the loop, `esi` never advanced) rather than
//  walked as an array, so this broadcasts the single dword `*items` into
//  every new slot of `this->m_data` (`this+0x0C`) from index `oldCount` up
//  to `oldCount + newCount`, clamped so it never runs past `this->m_cap`
//  (`this+0x1C`) - not an element-wise array copy.
//============================================================================
#include "agentbaseabs_related.h"

void* CAgentBaseAbsSlot3Arg::LookUp_6F4A45E0()
{
    return *(void**)((char*)this + 0x0C);
}

void CAgentBaseAbsSlot3Result::FixUp_6F4A45B0(void* outerAgent)
{
    char* self = (char*)this;
    char* node = (char*)outerAgent;
    *(void**)(node + 4) = self;
    *(void**)(node + 8) = *(void**)(self + 8);
    void* next = *(void**)(self + 8);
    if (next != 0)
    {
        *(void**)((char*)next + 4) = node;
    }
    *(void**)(self + 8) = node;
}

void CRelatedAgentsTableInsertExternal::InsertAt_6F4A4AD0(unsigned int oldCount, const void* items, unsigned int newCount)
{
    char* self = (char*)this;
    unsigned int cap = *(unsigned int*)(self + 0x1C);
    unsigned int end = oldCount + newCount;
    if (end >= cap)
    {
        end = cap;
    }
    if (oldCount < end)
    {
        void* const* src = (void* const*)items;
        void** data = *(void***)(self + 0x0C);
        unsigned int i = oldCount;
        do
        {
            void* v = *src;
            data[i] = v;
            i = i + 1;
        } while (i < end);
    }
}
