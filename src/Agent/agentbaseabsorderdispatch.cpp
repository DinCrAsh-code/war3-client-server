//============================================================================
//  0x6F4A59F0 / 0x6F4A5850 - CAgentBaseAbsOrderDispatch::MaybeDispatchOrder
//  and its tail-called dispatcher.
//
//  **MaybeDispatchOrder's one real call site (CAgentBaseAbs's own vtable
//  slot 17, agentbaseabsslot17.cpp) always passes a hard-coded 0 for
//  `orderBase`, so that call is a guaranteed no-op** - the function itself
//  is a completely ordinary "if present, adjust and dispatch" helper, just
//  never actually exercised from the one place in this call tree that
//  reaches it.  Kept exactly as written per CLAUDE.md's "say why when the
//  source looks odd" rule; nothing here is dead code in general, only at
//  that one call site.
//
//  DispatchOrder walks a `0x2C`-byte-stride array of up to
//  `orderList[0x1C]` entries (a plain count, not the related-agents table's
//  own count - a different array entirely, reached through the
//  already-adjusted `orderBase`/`orderList` pointer), switching on each
//  entry's own `+0x08` byte on six cases that all resolve to
//  `NIpse::CRlAgent`'s own already-reconstructed accessors
//  (Pathfinding/crlagent.h/agentbaseabs_relatedaccessors.cpp - moved there
//  from an earlier, mistaken `CAgentBaseAbsRelated` scoping, see that
//  header's own CORRECTION note) - the argument counts line up exactly
//  with those methods' own signatures.  Any other selector value is a
//  silent no-op (no `default` case, nothing after the switch).
//============================================================================
#include "crlagent.h"
#include "agentbaseabs_related.h"

struct CAgentBaseAbsOrderDispatch
{
    void MaybeDispatchOrder(void* orderBase, void* table);
    void DispatchOrder(void* orderList, void* table);
};

void CAgentBaseAbsOrderDispatch::MaybeDispatchOrder(void* orderBase, void* table)
{
    if (orderBase == 0)
        return;
    DispatchOrder((char*)orderBase + 0x108, table);
}

void CAgentBaseAbsOrderDispatch::DispatchOrder(void* orderList, void* table)
{
    NIpse::CRlAgent* self = (NIpse::CRlAgent*)this;
    char* list = (char*)orderList;

    unsigned int i = 0;
    while (i < *(unsigned int*)(list + 0x1C))
    {
        char* array = *(char**)(list + 0x0C);
        char* entry = array + i * 0x2C;
        unsigned int selector = *(unsigned int*)(entry + 8);

        switch (selector)
        {
        case 0:
            self->SubmitRelatedAgentEvent(entry, table);
            break;
        case 1:
            self->Attach(*(unsigned int*)(entry + 0x0C), *(unsigned int*)(entry + 0x10));
            break;
        case 2:
            self->DetachIndex(*(unsigned int*)(entry + 0x0C), *(int*)(entry + 0x10));
            break;
        case 3:
            self->SetField(*(unsigned int*)(entry + 0x0C), *(unsigned int*)(entry + 0x10),
                            *(unsigned int*)(entry + 0x14));
            break;
        case 4:
            self->DetachIndexEndpointBIfLinked(*(unsigned int*)(entry + 0x0C));
            break;
        case 5:
            self->SetEventId(*(unsigned int*)(entry + 0x0C), *(unsigned int*)(entry + 0x10));
            break;
        }

        i += 1;
    }
}
