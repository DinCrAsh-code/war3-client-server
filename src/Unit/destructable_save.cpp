//============================================================================
//  CDestructable's four persistence slots: Save/Load (14/15, full state)
//  and SaveFlags/LoadFlags (18/19, the lighter word-plus-life-value path
//  every class in this chain also carries - CAgentWar3's own SaveFlags/
//  LoadFlags, agentwar3_flags.cpp).
//
//  Save/Load's own five fields (+0x54, +0x55, +0x5C, +0x80, +0x84) are
//  plain scalars this call tree writes/reads once each and never sees
//  interpreted any further - no name beyond their offset is recoverable,
//  per CLAUDE.md's "do not invent fields the call tree does not read".
//============================================================================
#include "destructable.h"
#include "cdatastore.h"
#include "cdatastorescratch.h"

//  0x6F6EED50/0x6F6EFE00 - cdatastore_readcfloat.cpp/cdatastore_writecfloat.cpp.
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);
CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store,
                                          const CFloat* value);

//  0x6F266D80/0x6F266620 - destructable_versiongate.cpp.
struct SVersionGate;
SVersionGate* __fastcall ConfigureFloat5CGate(int since, int until, void* target);
CDataStore* __fastcall LoadFieldWithVersionGate(CDataStore* store, SVersionGate* gate);

//----------------------------------------------------------------------------
//  0x6F2662A0 - slot 14 (+0x38), Save.
//----------------------------------------------------------------------------
void CDestructable::Save(CDataStoreScratch* store)
{
    CSelectable::Save(store);

    store->WriteByte(m_reserved54);
    store->WriteByte(m_reserved55);
    store->WriteFloat(m_reservedFloat80);
    store->WriteDwordAlt3(m_positionNotifyHandle);
    store->WriteFloat(m_reservedFloat5C);
}

//----------------------------------------------------------------------------
//  0x6F267B20 - slot 15 (+0x3C), Load.  Version-gated base: a stream at or
//  after 0x177F (6015) carries a CSelectable to load; an older one only
//  ever had a CWidget there, so this falls back to CWidget::Load
//  (widget's own base body, agentdefaults.cpp/widget.h) instead - the same
//  "old stream, narrower base" shape a version counter this call tree
//  never otherwise interprets any further than the one comparison.
//----------------------------------------------------------------------------
void CDestructable::Load(CDataStore* store)
{
    if (store->GetVersion() >= 0x177F)
        CSelectable::Load(store);
    else
        CWidget::Load(store);

    store->ReadByte(&m_reserved54);
    store->ReadByte(&m_reserved55);
    store->ReadFloat(&m_reservedFloat80);
    store->ReadDwordAlt2(&m_positionNotifyHandle);

    LoadFieldWithVersionGate(store,
        ConfigureFloat5CGate(0x177F, -1, &m_reservedFloat5C));
}

//----------------------------------------------------------------------------
//  0x6F267380 - slot 18 (+0x48), SaveFlags.  CAgentWar3's own body
//  (agentwar3_flags.cpp) inlined, plus this class's own "life" triple.
//----------------------------------------------------------------------------
void CDestructable::SaveFlags(CDataStoreScratch* store)
{
    store->WriteDword(m_flags);

    CFloat value = m_floatB.m_value;
    WriteCFloat(store, &value);

    const CFloat* pair = &m_floatB.m_min;
    WriteCFloat(store, pair);
    WriteCFloat(store, pair + 1);
}

//----------------------------------------------------------------------------
//  0x6F2673C0 - slot 19 (+0x4C), LoadFlags.  CAgentWar3::LoadFlags's own
//  body (agentwar3_flags.cpp) inlined, plus the matching read-back and
//  republish (flag 0, "loading" rather than "changing") of the life
//  triple - the same shape item_load.cpp's own CItem::Load uses for its
//  FloatMiniB.
//----------------------------------------------------------------------------
void CDestructable::LoadFlags(CDataStore* store)
{
    unsigned int flags;
    store->ReadDword(&flags);
    m_flags = flags;

    unsigned int scratch;
    FloatMiniB* range = &m_floatB;
    ReadCFloat(store, (CFloat*)&scratch);
    ReadCFloat(store, &range->m_min);
    ReadCFloat(store, &range->m_max);
    ((FloatMiniB::NotifyFn)(*(void***)range)[0])(range, (CFloat*)&scratch, 0);
}
