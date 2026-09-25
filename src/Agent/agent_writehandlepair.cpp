//============================================================================
//  0x6F2D0350 - append a CAgent's {handle, typeTag} pair to a save stream,
//  writing -1 for each when the slot is empty.
//
//  The pointer arrives by address (`mov esi, [edx]`) rather than as the
//  agent itself: the caller owns a field holding the agent and hands over
//  that field, so the load happens here.
//
//  Two things about the shape are the shipped code's and not a choice.  The
//  null test is written twice - once per dword - rather than as one `if`
//  around a two-write block, because that is what the shipped stream does:
//  it re-tests `esi` after the first WriteDword returns, and its two tails
//  are separate `retn`s.  And the store pointer is never reloaded across
//  either call, because `ecx` still holds it: WriteDword returns `this`, and
//  the shipped code ignores that return and re-sets ecx from its own saved
//  copy in edi.
//
//  Own translation unit: both writes are real calls into the stream layer.
//============================================================================
#include "agent.h"
#include "cdatastorescratch.h"

CDataStoreScratch* __fastcall WriteAgentHandlePair(CDataStoreScratch* store,
                                                   CAgent** slot)
{
    CAgent* agent = *slot;

    store->WriteDword(agent != 0 ? agent->m_handle : 0xFFFFFFFFu);
    store->WriteDword(agent != 0 ? agent->m_typeTag : 0xFFFFFFFFu);
    return store;
}
