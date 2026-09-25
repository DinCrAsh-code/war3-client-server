//============================================================================
//  0x6F4A4F80 - NIpse::CRlAgentDef::Load, vtable slot 10 (+0x28).  The base
//  `CAgentRelation::Load` first, then this class's own +0x108 array
//  (LoadRecordArray, sub_6F4A4900 - same cross-batch naming seam as
//  Save's own SaveRecordArray, rlagentdef_save.cpp), then the same
//  global-registry link Apply performs (rlagentdef_apply.cpp).
//============================================================================
#include "rlagentdef.h"
#include "agentrelation.h"
#include "game.h"

void __fastcall LoadRecordArray(void* reader, void* array);

namespace NIpse {

void CRlAgentDef::Load(void* reader)
{
    ((CAgentRelation*)this)->Load(reader);

    LoadRecordArray(reader, (char*)this + 0x108);

    ((SRegistryRoot*)((char*)g_pTimeSync + 0x25C))->LinkNodeIntoRegistry(this);
}

}  // namespace NIpse
