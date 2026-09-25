//============================================================================
//  0x6F3A0950 - one of three /OPT:ICF-less duplicates of the identical body
//  (this image was linked without /OPT:ICF - agentdefaults.cpp); each is
//  its own real out-of-line function in the shipped binary, so each gets
//  its own translation unit here too - a `static` (or even non-static)
//  definition living in the same TU as its caller scored badly (~0.44 on
//  CGameWar3::Save) because /Ob2 inlined it at every one of its ~30 call
//  sites instead of leaving a real call, the same class of gap CLAUDE.md's
//  own house rules call out ("put a callee in its own TU rather than
//  reaching for __declspec(noinline)").
//
//  `ref` points at a `SCheckedGameSlot`-shaped one-field wrapper
//  (agenttypedslots.h - a bare `CAgent* m_value`), i.e. `*ref` is the
//  agent or null.  The two dwords written are exactly
//  `CAgent::m_handle`/`CAgent::m_typeTag` (agent.h, +0xC/+0x10) - the same
//  identity pair PlayerRecordChecksumHash's own array walk already hashes
//  (Player/playerrecordchecksumhash.cpp) - or -1/-1 for an empty slot.
//  Written as two independent ternaries because that is what the shipped
//  stream does: it tests the agent pointer twice rather than sharing one
//  branch between both writes.  Called from CGameWar3::Save
//  (gamewar3save.cpp) for `m_gameSlotValue`.
//============================================================================
#include "cdatastorescratch.h"
#include "agent.h"

CDataStoreScratch* __fastcall SerializeGameSlotRef(CDataStoreScratch* stream,
                                                     CAgent* const* ref)
{
    CAgent* agent = *ref;

    stream->WriteDword(agent ? agent->m_handle   : (unsigned int)-1);
    stream->WriteDword(agent ? agent->m_typeTag  : (unsigned int)-1);

    return stream;
}
