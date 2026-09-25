//============================================================================
//  0x6F4975E0 - NIpse::CRlProperty::Save, vtable slot 9 (+0x24).
//
//  The base `CAgentRelation::Save` first, then this class's own two
//  relation-link handles at +0x50/+0x60 (SaveLinkRefIndirect,
//  agentrelationsavelinkrefindirect.cpp), then its own CFloat/dword pair at
//  +0x70/+0x74.
//============================================================================
#include "rlproperty.h"
#include "agentrelation.h"
#include "cdatastorescratch.h"
#include "CFloat.h"

void __fastcall SaveLinkRefIndirect(CDataStoreScratch* store, void* handle);
CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);

namespace NIpse {

void CRlProperty::Save(void* writer)
{
    CDataStoreScratch* store = (CDataStoreScratch*)writer;

    ((CAgentRelation*)this)->Save(writer);

    SaveLinkRefIndirect(store, (char*)this + 0x50);
    SaveLinkRefIndirect(store, (char*)this + 0x60);

    unsigned int value = *(const unsigned int*)((char*)this + 0x74);
    CDataStoreScratch* chain = WriteCFloat(store, (const CFloat*)((char*)this + 0x70));
    chain->WriteDword(value);
}

}  // namespace NIpse
