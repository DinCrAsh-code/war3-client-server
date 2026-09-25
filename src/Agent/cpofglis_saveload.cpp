//============================================================================
//  0x6F4A8D00 / 0x6F4A8D20 - NIpse::CPoFgLis::Save/Load, vtable slots 9/10.
//  Both call the base's own Save/Load first (Agent/agentrelation.h,
//  0x6F4A6570/0x6F4A6C10), then stream this class's own +0x50 mask field -
//  same shape Agent/poseparate_save.cpp/poseparate_load.cpp's own
//  CPoSeparate::Save/Load already establish for a sibling class's own added
//  field, one offset column over (WriteDwordAlt2/ReadDwordAlt3 instead of
//  WriteCFloat/ReadCFloat, since m_mask is a plain dword, not a CFloat).
//============================================================================
#include "cpofglis.h"
#include "agentrelation.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"

namespace NIpse {

void CPoFgLis::Save(void* writer)
{
    ((CAgentRelation*)this)->Save(writer);
    ((CDataStoreScratch*)writer)->WriteDwordAlt2(m_mask);
}

void CPoFgLis::Load(void* reader)
{
    ((CAgentRelation*)this)->Load(reader);
    ((CDataStore*)reader)->ReadDwordAlt3(&m_mask);
}

}  // namespace NIpse
