//============================================================================
//  0x6F497620 - NIpse::CRlProperty::Load, vtable slot 10 (+0x28), Save's
//  exact mirror image.
//============================================================================
#include "rlproperty.h"
#include "agentrelation.h"
#include "cdatastore.h"
#include "CFloat.h"

void __fastcall LoadRelationLink(CDataStore* store, void* slot);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

namespace NIpse {

void CRlProperty::Load(void* reader)
{
    CDataStore* store = (CDataStore*)reader;

    ((CAgentRelation*)this)->Load(reader);

    LoadRelationLink(store, (char*)this + 0x50);
    LoadRelationLink(store, (char*)this + 0x60);

    CDataStore* chain = ReadCFloat(store, (CFloat*)((char*)this + 0x70));
    chain->ReadDword((unsigned int*)((char*)this + 0x74));
}

}  // namespace NIpse
