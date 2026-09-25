//============================================================================
//  NIpse::CPoPosBh - vtable slots 9 (Save, 0x6F4AAEE0) and 10 (Load,
//  0x6F4AB480). See poposbh.h for the class-level header comment.
//
//  Resolved this follow-up session (both were `TODO`, left by the earlier
//  cluster-H-remainder session as "own simple nine-CFloat half is real, but
//  continues into the +0xA8/+0xB8 related-agent array and +0xBC
//  registration re-derivation with several unresolved pieces" - see
//  `docs/targets/NTempestNIpse_clusterH_remainder.md`). The three "several
//  more genuinely new leaves" that document named as unresolved
//  (0x6F4AB050/0x6F483270/0x6F4A6A40) turned out to be code this repo
//  ALREADY carries, real, under other classes' own names - not new work at
//  all, once grepped for by address rather than assumed novel:
//
//    - 0x6F4AB050 IS `SBhPoTickRelaySlotArray::Grow`
//      (Missile/bhpoprojectile_slots.cpp) - already real, shared code with
//      `CBhPoProjectile`, confirmed by `Load`'s own call reaching it with
//      `this = &m_relatedAgents` (this class's own +0x9C array, see
//      poposbh.h's own updated header comment);
//    - 0x6F483270 IS `CTimeSync::GetChannel` (Sync/CSyncChannel.cpp,
//      already `EXACT`) - pick the local or the remote of `g_pTimeSync`'s
//      own two embedded channels off bit 31 of the object's own +0x14
//      flags word;
//    - 0x6F4A6A40 IS `RebuildSyncRegistration`
//      (Agent/agentrelationrebuildsyncrecord.cpp, already `DIFFERS`,
//      claimed by another concurrent session's own worktree entry when
//      this one went to claim it - a genuine collision, resolved by
//      reusing rather than re-deriving) - reads the same seven fields
//      `Save`'s own tail writes, in the same order, and re-derives the
//      `SSyncRegistration*` through `CSyncChannel::MakeRequest` (game.h).
//      `CAgentRelation::Load` (agentrelationsyncload.cpp) already calls
//      both of these the exact same way this file's own `Load` now does.
//============================================================================
#include "poposbh.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"

CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

//  0x6F4AEF40 - already reconstructed elsewhere (Pathfinding/pathclient_save.cpp).
void __fastcall WritePathRefEntry(CDataStoreScratch* store, const void* raw);

//  0x6F4AF090 - already reconstructed elsewhere (Pathfinding/pathref_load.cpp).
struct SPathRefReader
{
    void* ReadEntry();
};

//  0x6F4A6A40 - already reconstructed, Agent/agentrelationrebuildsyncrecord.cpp
//  (same local-redeclaration convention that file's own caller,
//  agentrelationsyncload.cpp, already uses rather than a shared header).
SSyncRegistration* __fastcall RebuildSyncRegistration(
    CDataStore* store, CSyncChannel* channel, void* owner);

namespace NIpse {

//----------------------------------------------------------------------------
//  0x6F4AAEE0 - vtable slot 9, Save. Base `CPoPos::Save` first, then this
//  class's own nine added CFloats (+0xC0 through +0xE0, offset order), the
//  +0x9C related-agent array's own live count and every entry (raw pointer
//  value, via `WritePathRefEntry` - same "handle by value" convention
//  `CPoPos::Save` already uses for its own +0x94/+0x98), then a "have a
//  sync registration" flag and, if set, that `SSyncRegistration`'s own
//  m_deadline/m_value/m_sequence/m_time/"sample me" bit.
//----------------------------------------------------------------------------
void CPoPosBh::Save(CDataStoreScratch* store)
{
    ((CPoPos*)this)->Save(store);

    //  Chained through each call's own returned `this` from here through
    //  the count write - matches the shipped stream's own register reuse.
    //  The array loop below deliberately goes back to the ORIGINAL `store`
    //  rather than the chain - so does the shipped code (its own `ebx`
    //  copy of the untouched pointer, not the running `eax` chain).
    CDataStoreScratch* chain = WriteCFloat(store, &m_pdl);
    chain = WriteCFloat(chain, &m_forField);
    chain = WriteCFloat(chain, &m_rot);
    chain = WriteCFloat(chain, &m_acc);
    chain = WriteCFloat(chain, &m_ang);
    chain = WriteCFloat(chain, &m_lvl);
    chain = WriteCFloat(chain, &m_ldl);
    chain = WriteCFloat(chain, &m_brk);
    chain = WriteCFloat(chain, &m_prw);
    chain->WriteDword(m_relatedAgents.m_count);

    void* const* entries = (void* const*)m_relatedAgents.m_storage.m_data;
    for (unsigned int i = 0; i < m_relatedAgents.m_count; ++i)
        WritePathRefEntry(store, entries[i]);

    SSyncRegistration* reg = m_syncReg;
    if (reg != 0)
    {
        chain = store->WriteDword(1);
        chain = WriteCFloat(chain, &reg->m_deadline);
        chain = WriteCFloat(chain, &reg->m_value);
        chain = chain->WriteDword((unsigned int)reg->m_sequence);
        chain = chain->WriteDwordAlt2((unsigned int)reg->m_time);
        chain->WriteDword(reg->m_flags & 1);
    }
    else
    {
        store->WriteDword(0);
    }
}

//----------------------------------------------------------------------------
//  0x6F4AB480 - vtable slot 10, Load. Save's near-mirror for the nine
//  CFloats and the related-agent array (read back one
//  `SPathRefReader::ReadEntry()` at a time, appended through
//  `SBhPoTickRelaySlotArray::Grow` - `Grow`'s own `fillValue` argument is
//  the address of a local holding the just-read pointer, matching the
//  shipped stream's own stack-slot reuse for that scratch value), but the
//  +0xBC registration is NOT streamed the way `CPoPos::Load`'s own
//  +0x94/+0x98 handles are - it is re-derived through
//  `RebuildSyncRegistration` against whichever of `g_pTimeSync`'s own two
//  embedded channels `CTimeSync::GetChannel` picks for this object, the
//  exact same two-call pattern `CAgentRelation::Load` already uses.
//----------------------------------------------------------------------------
void CPoPosBh::Load(CDataStore* store)
{
    ((CPoPos*)this)->Load(store);

    CDataStore* chain = ReadCFloat(store, &m_pdl);
    chain = ReadCFloat(chain, &m_forField);
    chain = ReadCFloat(chain, &m_rot);
    chain = ReadCFloat(chain, &m_acc);
    chain = ReadCFloat(chain, &m_ang);
    chain = ReadCFloat(chain, &m_lvl);
    chain = ReadCFloat(chain, &m_ldl);
    chain = ReadCFloat(chain, &m_brk);
    chain = ReadCFloat(chain, &m_prw);

    unsigned int count = 0;
    chain->ReadDword(&count);

    for (unsigned int i = 0; i < count; ++i)
    {
        void* entry = ((SPathRefReader*)store)->ReadEntry();
        m_relatedAgents.Grow(&entry, 1);
    }

    CSyncChannel* channel = (CSyncChannel*)g_pTimeSync->GetChannel((const CHandleObject*)this);
    m_syncReg = RebuildSyncRegistration(store, channel, this);
}

}  // namespace NIpse
