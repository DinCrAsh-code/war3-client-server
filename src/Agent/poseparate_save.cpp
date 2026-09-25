//============================================================================
//  0x6F495070 - NIpse::CPoSeparate::Save, vtable slot 11. Streams m_sepvX,
//  m_sepvY (each via WriteCFloat, Net/cdatastore_writecfloat.cpp) then
//  m_opt (via CDataStoreScratch::WriteDwordAlt2,
//  Net/cdatastorescratch_writedwordalt2.cpp) into the caller's
//  CDataStoreScratch, in that offset order.
//============================================================================
#include "poseparate.h"
#include "cdatastorescratch.h"

CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);

namespace NIpse {

void CPoSeparate::Save(void* store)
{
    CDataStoreScratch* s = (CDataStoreScratch*)store;
    CFloat* field = &m_sepvX;
    WriteCFloat(s, field);
    WriteCFloat(s, field + 1);
    s->WriteDwordAlt2(m_opt);
}

}  // namespace NIpse
