//============================================================================
//  0x6F4A8BD0 / 0x6F4A8C20 - NIpse::CPoFgPulseMod::Save/Load, vtable slots
//  9/10. CAgentRelation::Save/Load first, then all five fields (m_mask,
//  m_delta via WriteDwordAlt2/ReadDwordAlt3; m_frequency via
//  WriteCFloat/ReadCFloat; m_duration, m_playfield via plain
//  WriteDword/ReadDword), each chained through the previous call's own
//  returned pointer.
//============================================================================
#include "cpofgpulsemod.h"
#include "agentrelation.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"

CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

namespace NIpse {

void CPoFgPulseMod::Save(void* writer)
{
    ((CAgentRelation*)this)->Save(writer);

    CDataStoreScratch* s = (CDataStoreScratch*)writer;
    s = s->WriteDwordAlt2(m_mask);
    s = s->WriteDwordAlt2(m_delta);
    s = WriteCFloat(s, &m_frequency);
    s = s->WriteDword(m_duration);
    s->WriteDword(m_playfield);
}

void CPoFgPulseMod::Load(void* reader)
{
    ((CAgentRelation*)this)->Load(reader);

    CDataStore* s = (CDataStore*)reader;
    s = s->ReadDwordAlt3(&m_mask);
    s = s->ReadDwordAlt3(&m_delta);
    s = ReadCFloat(s, &m_frequency);
    s = s->ReadDword(&m_duration);
    s->ReadDword(&m_playfield);
}

}  // namespace NIpse
