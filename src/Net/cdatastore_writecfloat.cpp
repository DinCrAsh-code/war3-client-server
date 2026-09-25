//============================================================================
//  0x6F6EFE00 - write one CFloat into a CDataStoreScratch, by address.
//
//  It sits at 0x6F6EFE00, sixteen bytes below CFloat::operator/ - i.e. in
//  the software-float module, not the streaming one, which is why it takes
//  its value as a `const CFloat*` in edx rather than being a method on the
//  store.
//
//  Own translation unit: the call into WriteDwordAlt is real, and its four
//  callers (CWidget::Save, SPathRefArray::Save) reach this with real calls
//  too.
//============================================================================
#include "cdatastorescratch.h"
#include "CFloat.h"

CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store,
                                           const CFloat* value)
{
    store->WriteDwordAlt((unsigned int)value->m_bits);
    return store;
}
