//============================================================================
//  0x6F46EBE0 - AgileAgentQuery's own vtable slot 0 (its only slot).  See
//  agentquery.h's own declaration comment for the full reasoning: this
//  restamps the immediate base's own vtable literal
//  (NIpse::CPmQuery<AgileAgentFilter>, 0x6F951844) rather than a distinct
//  AgileAgentQuery vtable, the same "no vtable-stamping constructor for the
//  correct derived class was ever reconstructed, so the compiler's own
//  immediate is whatever base it last had" shape
//  Misc/ntempest_cpmquery_scalardtors.cpp's own CPoQuery/CSeQuery already
//  document one level down this same hierarchy.
//============================================================================
#include "agentquery.h"
#include "cellbuffer.h"
#include "storm.h"

//  See Pathfinding/crlagent_dtor.cpp's own StampVtable for why this goes
//  through a volatile store rather than a plain literal assignment.
static __forceinline void StampVtable(void* pThis, void* vtbl)
{
    *(void* volatile*)pThis = vtbl;
}

void* AgileAgentQuery::ScalarDeletingDestructor(unsigned int flags)
{
    unsigned int count = m_count;

    StampVtable(this, (void*)0x6F951844);

    if (count != 0)
        RemoveEntries(0, count);

    ReleaseCMemBlockBase(this);

    if (flags & 1)
        SMemFree(this, "delete", -1, 0);

    return this;
}
