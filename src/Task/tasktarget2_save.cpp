//============================================================================
//  CTaskTarget2's four serialisation-family vtable overrides -
//  GetRecordVersion, DumpState, SaveFlags, LoadFlags - all clustered at
//  0x6F2889xx-0x6F288Axx, one module, one TU.  Unlike CTaskTarget itself,
//  this class does *not* re-override Save/Load (tasktarget2.h): it adds
//  only a plain SHandleTriple beyond CTaskTarget's own end, with no
//  handle-resolution work of its own for the record-save path, so
//  CTaskTarget::Save/Load (inherited unchanged) already cover it.
//============================================================================
#include "tasktarget2.h"
#include "../Net/cdatastorescratch.h"
#include "../Net/cdatastore.h"
#include "../Agent/handletriple.h"
#include "../Storm/textsink.h"

//----------------------------------------------------------------------------
//  0x6F2889A0 - slot 10, GetRecordVersion.  `retn 4`.  Same shape as
//  CTaskTarget's own: a plain base-class call (not virtual dispatch -
//  straight to CAgent::GetRecordVersion, skipping every intermediate
//  class's own override, exactly as CTaskTarget's own body does) plus this
//  class's own constant.
//----------------------------------------------------------------------------
int CTaskTarget2::GetRecordVersion(int context)
{
    return CAgent::GetRecordVersion(context) + 6;
}

//----------------------------------------------------------------------------
//  0x6F2889F0 - slot 17, DumpState.  `retn 4`.  CTaskTarget's own five
//  dumped fields (m_flags/m_field24+28/m_field30, the second SHandleTriple's
//  {m_field34,m_field38} pair, m_fieldWeight) duplicated rather than reached
//  through a base-class call - same shape CTaskTarget::DumpState takes, one
//  real call per field rather than one call into CTaskTarget::DumpState -
//  then this class's own third SHandleTriple's {m_field4C,m_field50} pair
//  (m_field54 not separately dumped, same as m_field2C/m_field3C above it).
//----------------------------------------------------------------------------
void CTaskTarget2::DumpState(void* sink)
{
    reinterpret_cast<SIntMiniValue*>(&m_flags)->DumpTo(sink);
    reinterpret_cast<SRefMiniValue*>(&m_field24)->DumpTo(sink);
    reinterpret_cast<SIntMiniValue*>(&m_field30)->DumpTo(sink);
    reinterpret_cast<SRefMiniValue*>(&m_field34)->DumpTo(sink);
    m_fieldWeight.DumpTo(sink);
    reinterpret_cast<SRefMiniValue*>(&m_field4C)->DumpTo(sink);
}

//----------------------------------------------------------------------------
//  0x6F2889B0 - slot 18, SaveFlags.  `retn 4`.  A real out-of-line call into
//  CTaskTarget::SaveFlags(store) first (unlike DumpState above, this one
//  *does* reach the base through a real call - the shipped body's own
//  `call sub_6F288790` target, CTaskTarget's own slot 18), then this class's
//  own third triple's three dwords: +0x4C/+0x50 via WriteDword, +0x54 via
//  WriteDwordAlt2 - the same Alt2-on-the-"extra"-word shape the two triples
//  above it use.
//----------------------------------------------------------------------------
void CTaskTarget2::SaveFlags(CDataStoreScratch* store)
{
    CTaskTarget::SaveFlags(store);

    store->WriteDword(m_field4C);
    store->WriteDword(m_field50);
    store->WriteDwordAlt2(m_field54);
}

//----------------------------------------------------------------------------
//  0x6F288A50 - slot 19, LoadFlags.  `retn 4`.  Mirrors SaveFlags: a real
//  out-of-line call into CTaskTarget::LoadFlags(store) first, then this
//  class's own third triple read in one ReadHandleTriple(store,
//  (SHandleTriple*)&m_field4C) call.
//----------------------------------------------------------------------------
void CTaskTarget2::LoadFlags(CDataStore* store)
{
    CTaskTarget::LoadFlags(store);
    ReadHandleTriple(store, reinterpret_cast<SHandleTriple*>(&m_field4C));
}
