//============================================================================
//  0x6F4A8A20 / 0x6F4A8A40 - NIpse::CPoFlag::Save/Load, vtable slots 9/10.
//  CRlProperty::Save/Load first, then this class's own m_value via
//  CDataStoreScratch::WriteDwordAlt2 / CDataStore::ReadDwordAlt3.
//============================================================================
#include "cpoflag.h"
#include "rlproperty.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"

namespace NIpse {

void CPoFlag::Save(void* writer)
{
    ((CRlProperty*)this)->Save(writer);
    ((CDataStoreScratch*)writer)->WriteDwordAlt2(m_value);
}

void CPoFlag::Load(void* reader)
{
    ((CRlProperty*)this)->Load(reader);
    ((CDataStore*)reader)->ReadDwordAlt3(&m_value);
}

}  // namespace NIpse
