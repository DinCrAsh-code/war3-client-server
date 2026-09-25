//============================================================================
//  CTaskPoint2's four remaining serialisation-family vtable overrides -
//  GetRecordVersion, DumpState, SaveFlags, LoadFlags - all clustered at
//  0x6F2882xx-0x6F2883xx, one module, one TU.  Unlike CTaskPoint (which
//  re-overrides Save/Load too, taskpoint_save.cpp), CTaskPoint2 inherits
//  CTaskPoint::Save/CTaskPoint::Load unchanged - confirmed directly by
//  diffing both classes' own vtable slots 14/15 (taskpoint2.h) - since its
//  own two new fields are plain FloatMini-shaped values with no network
//  handle of their own, the same kind of state CTaskPoint's own point
//  FloatMinis already round-trip purely through
//  SaveFlags/LoadFlags/DumpState rather than through Save/Load.
//============================================================================
#include "taskpoint2.h"
#include "../Net/cdatastorescratch.h"
#include "../Net/cdatastore.h"
#include "../Storm/textsink.h"

//  0x6F6EFE00/0x6F6EED50 - Net/cdatastore_writecfloat.cpp,
//  cdatastore_readcfloat.cpp.  Forward-declared locally, same convention
//  taskpoint_save.cpp already uses for these two.
CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store,
                                           const CFloat* value);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

//----------------------------------------------------------------------------
//  0x6F288280 - slot 10, GetRecordVersion.  `retn 4`.  Same shape as
//  CTaskPoint's own (0x6F288030): a plain base-class call (not virtual
//  dispatch - straight to CAgent::GetRecordVersion, skipping CTask's and
//  CTaskPoint's own overrides) plus this class's own constant.
//----------------------------------------------------------------------------
int CTaskPoint2::GetRecordVersion(int context)
{
    return CAgent::GetRecordVersion(context) + 8;
}

//----------------------------------------------------------------------------
//  0x6F2882D0 - slot 17, DumpState.  `retn 4`.  CTaskPoint's own six fields
//  (m_flags/m_field24+28/m_field30/m_pointX/m_pointY/m_pointZ, duplicated
//  rather than reached through a base-class call - same shape the shipped
//  code takes, one real call per field rather than one call into
//  CTaskPoint::DumpState), then this class's own two point FloatMini-shaped
//  blocks in offset order - reinterpret_cast to `FloatMini*` at the point
//  of use, same convention taskrotate.h's own m_field34/m_field38 pair
//  already establishes for an untyped FloatMini-shaped field (taskpoint2.h's
//  own header comment explains why these two are untyped storage).
//----------------------------------------------------------------------------
void CTaskPoint2::DumpState(void* sink)
{
    reinterpret_cast<SIntMiniValue*>(&m_flags)->DumpTo(sink);
    reinterpret_cast<SRefMiniValue*>(&m_field24)->DumpTo(sink);
    reinterpret_cast<SIntMiniValue*>(&m_field30)->DumpTo(sink);
    m_pointX.DumpTo(sink);
    m_pointY.DumpTo(sink);
    m_pointZ.DumpTo(sink);
    reinterpret_cast<FloatMini*>(&m_pointX2Vtable)->DumpTo(sink);
    reinterpret_cast<FloatMini*>(&m_pointY2Vtable)->DumpTo(sink);
}

//----------------------------------------------------------------------------
//  0x6F288290 - slot 18, SaveFlags.  `retn 4`.  CTaskPoint::SaveFlags(store)
//  (the inherited-from-CTask-plus-CTaskPoint body, unmodified - a real
//  out-of-line call, not inlined) first, then this class's own two point
//  values through WriteCFloat - the shipped call site copies each field to
//  a stack temp and takes *that* address (`mov eax,[esi+54] / mov
//  [esp+N],eax / lea edx,[esp+N]`), not the field's own address directly,
//  same read-into-local-then-address-of-local shape CTaskPoint's own
//  SaveFlags already uses for its three (taskpoint_save.cpp) - passing
//  `&m_pointX2` directly would alias `this` across the call instead.
//----------------------------------------------------------------------------
void CTaskPoint2::SaveFlags(CDataStoreScratch* store)
{
    CTaskPoint::SaveFlags(store);

    CFloat value(kCFloatNoInit);
    value = m_pointX2;
    WriteCFloat(store, &value);
    value = m_pointY2;
    WriteCFloat(store, &value);
}

//  0x6F4770E0's own vtable slot 0 - see Math/floatmini.h's own note on
//  FloatMiniB::SetPair for why this is reached by raw offset rather than a
//  named virtual: it is a real "set value and notify" override, not the
//  placeholder destructor this reconstruction declares at that slot.
typedef void (__thiscall *FloatMiniNotifyFn)(void* self, CFloat* value,
                                              int flag);

//----------------------------------------------------------------------------
//  0x6F288340 - slot 19, LoadFlags.  `retn 4`.  Mirrors SaveFlags:
//  CTaskPoint::LoadFlags(store) first (a real out-of-line call, not
//  inlined), then this class's own two point values each read via
//  ReadCFloat into the same reused stack local (the shipped body's own
//  single reused scratch slot, same idiom taskpoint_save.cpp's own
//  CTaskPoint::LoadFlags already establishes) and then published through
//  each block's own vtable slot 0 - same LoadFloatMini shape, taking the
//  field by pointer so the compiler keeps its address in one register
//  across both the vtable fetch and the notify call: the shipped body
//  computes a fresh `lea` for this class's first new field but simply
//  advances its own `this` register (`add esi, 88h`) for the second,
//  because esi is otherwise dead by then - the same asymmetry
//  CTaskPoint::LoadFlags's own three-field version already produces from
//  this identical call shape.
//----------------------------------------------------------------------------
static void LoadFloatMini(void* field, CDataStore* store, CFloat* scratch)
{
    ReadCFloat(store, scratch);
    ((FloatMiniNotifyFn)(*(void***)field)[0])(field, scratch, 0);
}

void CTaskPoint2::LoadFlags(CDataStore* store)
{
    CTaskPoint::LoadFlags(store);

    unsigned int value;
    CFloat* scratch = reinterpret_cast<CFloat*>(&value);
    LoadFloatMini(&m_pointX2Vtable, store, scratch);
    LoadFloatMini(&m_pointY2Vtable, store, scratch);
}
