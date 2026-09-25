//============================================================================
//  0x6F4A4F60 - NIpse::CRlAgentDef::Save, vtable slot 9 (+0x24).  The base
//  `CAgentRelation::Save` first, then this class's own +0x108 array
//  (SaveRecordArray, sub_6F4A4850).
//
//  SaveRecordArray is declared only, not defined here - a cross-batch
//  naming seam (CLAUDE.md's own task assignment flagged this as expected):
//  its own leaf callees (0x6F4C2310/0x6F4C2760/0x6F4B3770-family writers
//  over the array's per-record fields) are out of this session's own scope
//  cap, left for whichever session reaches sub_6F4A4850 as one of its own
//  direct roots.  link_check.py will find the missing definition until
//  then, the same way agentrelation.h's own
//  `CAgentBaseAbsSlot3Externals::SetSomething_6F4A6070` seam already does.
//============================================================================
#include "rlagentdef.h"
#include "agentrelation.h"

void __fastcall SaveRecordArray(void* writer, void* array);

namespace NIpse {

void CRlAgentDef::Save(void* writer)
{
    ((CAgentRelation*)this)->Save(writer);

    SaveRecordArray(writer, (char*)this + 0x108);
}

}  // namespace NIpse
