//============================================================================
//  0x6F4950A0 - NIpse::CPoSeparate::Load, vtable slot 12. Save's exact
//  counterpart: reads m_sepvX, m_sepvY (each via ReadCFloat,
//  Net/cdatastore_readcfloat.cpp) then m_opt (via
//  CDataStore::ReadDwordAlt3, Net/cdatastore_readdwordalt2.cpp) from the
//  caller's CDataStore, in that same offset order.
//============================================================================
#include "poseparate.h"
#include "cdatastore.h"

CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

namespace NIpse {

void CPoSeparate::Load(void* store)
{
    CDataStore* s = (CDataStore*)store;
    CFloat* field = &m_sepvX;
    ReadCFloat(s, field);
    ReadCFloat(s, field + 1);
    s->ReadDwordAlt3(&m_opt);
}

}  // namespace NIpse
