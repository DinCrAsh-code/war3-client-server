//============================================================================
//  CTaskPoint's five serialisation-family vtable overrides - GetRecordVersion,
//  Save, Load, DumpState, SaveFlags, LoadFlags - all clustered at
//  0x6F2880xx-0x6F2882xx, one module, one TU.  Unlike CTaskParam/CTaskRotate
//  (which inherit CTask's own Save/Load unchanged), CTaskPoint re-overrides
//  Save/Load too, since it has real per-instance state (the m_moveRequest
//  handle) that needs its own round-trip on top of what CTask::Save/
//  CTask::Load already do for the base fields.
//============================================================================
#include "taskpoint.h"
#include "../Net/cdatastorescratch.h"
#include "../Net/cdatastore.h"
#include "../Agent/handletriple.h"
#include "../Agent/presenceref.h"
#include "../Game/game.h"
#include "../Storm/textsink.h"

//  0x6F6EFE00/0x6F6EED50 - Net/cdatastore_writecfloat.cpp,
//  cdatastore_readcfloat.cpp.  Forward-declared locally rather than pulled
//  in through a shared header - same convention Item/item_save.cpp and
//  item_load.cpp already use for these two.
CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store,
                                           const CFloat* value);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

//----------------------------------------------------------------------------
//  0x6F288030 - slot 10, GetRecordVersion.  `retn 4`.  Same shape as
//  CTask's/CTaskParam's own: a plain base-class call (not virtual dispatch -
//  straight to CAgent::GetRecordVersion, skipping CTask's own override)
//  plus this class's own constant.
//----------------------------------------------------------------------------
int CTaskPoint::GetRecordVersion(int context)
{
    return CAgent::GetRecordVersion(context) + 6;
}

//----------------------------------------------------------------------------
//  0x6F2881D0 - slot 14, Save.  `retn 4`.  CTask::Save(store) (the
//  inherited-from-CAgentWar3 body, unmodified - see task.h) first, then
//  this class's own m_moveRequest: if it resolves through LookupHandle,
//  write its real {m_handle, m_typeTag} pair; otherwise write {-1, -1}.
//----------------------------------------------------------------------------
void CTaskPoint::Save(CDataStoreScratch* store)
{
    CTask::Save(store);

    //  m_moveRequest inherits m_handle/m_typeTag from CPresenceRef -
    //  MovementRequest itself adds nothing else this call needs.
    MovementRequest* ref = m_moveRequest;

    if (ref != 0 &&
        LookupHandle(ref->m_handle, (int)ref->m_typeTag) != 0)
    {
        store->WriteDword(ref->m_handle);
        store->WriteDword(ref->m_typeTag);
    }
    else
    {
        store->WriteDword((unsigned int)-1);
        store->WriteDword((unsigned int)-1);
    }
}

//----------------------------------------------------------------------------
//  0x6F288230 - slot 15, Load.  `retn 4`.  CTask::Load(store) first, then
//  hand &m_moveRequest to sub_6F2871A0 - not reconstructed this session
//  (out of scope: it is the "resolve/allocate a MovementRequest and bind it
//  to the handle just read" path, a module of its own in the 0x6F287100
//  neighbourhood, no simpler than ConstructCTaskPoint's own SEH-framed
//  callee) - declared here only so the real call site compiles and matches.
//  `store` in ecx, `&m_moveRequest` in edx - fastcall, store as `this`.
//----------------------------------------------------------------------------
CDataStore* __fastcall LoadMovementRequestField(CDataStore* store,
                                                 void* fieldAddr);  // 0x6F2871A0, not reconstructed

void CTaskPoint::Load(CDataStore* store)
{
    CTask::Load(store);
    LoadMovementRequestField(store, &m_moveRequest);
}

//----------------------------------------------------------------------------
//  0x6F2880C0 - slot 17, DumpState.  `retn 4`.  CTask's own three fields
//  (m_flags/m_field24+28/m_field30, duplicated rather than reached through
//  a base-class call - same shape the shipped code takes, one real call per
//  field rather than one call into CTask::DumpState), then the three point
//  FloatMinis in offset order.
//----------------------------------------------------------------------------
void CTaskPoint::DumpState(void* sink)
{
    reinterpret_cast<SIntMiniValue*>(&m_flags)->DumpTo(sink);
    reinterpret_cast<SRefMiniValue*>(&m_field24)->DumpTo(sink);
    reinterpret_cast<SIntMiniValue*>(&m_field30)->DumpTo(sink);
    m_pointX.DumpTo(sink);
    m_pointY.DumpTo(sink);
    m_pointZ.DumpTo(sink);
}

//----------------------------------------------------------------------------
//  0x6F288040 - slot 18, SaveFlags.  `retn 4`.  CTask's own five fields
//  (+0x20..+0x30, duplicated the same way DumpState is - one real call per
//  field), then the three point FloatMinis' own CFloat values through
//  WriteCFloat.  All three share *one* local (the shipped code reuses the
//  same stack slot three times - `mov [esp+N],val / lea edx,[esp+N]` at
//  each of the three call sites, never a fresh `sub esp` for a second or
//  third one) - a single reused local matches that; three independent
//  locals cost an extra `sub esp,8`/`add esp,8` frame the dump does not
//  have.  WriteCFloat takes a `const CFloat*`, and passing
//  `&m_pointX.m_value` directly would alias `this` across the call instead
//  of the shipped code's own read-into-local-then-address-of-local shape
//  (the same reason FloatMiniB::DumpTo, Math/floatmini_dump.cpp, copies
//  m_min/m_max to locals before formatting them).
//----------------------------------------------------------------------------
void CTaskPoint::SaveFlags(CDataStoreScratch* store)
{
    store->WriteDword(m_flags);
    store->WriteDword(m_field24);
    store->WriteDword(m_field28);
    store->WriteDwordAlt2(m_field2C);
    store->WriteDword(m_field30);

    CFloat value(kCFloatNoInit);
    value = m_pointX.m_value;
    WriteCFloat(store, &value);
    value = m_pointY.m_value;
    WriteCFloat(store, &value);
    value = m_pointZ.m_value;
    WriteCFloat(store, &value);
}

//  0x6F4770E0's own vtable slot 0 - see Math/floatmini.h's own note on
//  FloatMiniB::SetPair for why this is reached by raw offset rather than
//  a named virtual: it is a real "set value and notify" override, not the
//  placeholder destructor this reconstruction declares at that slot.
typedef void (__thiscall *FloatMiniNotifyFn)(void* self, CFloat* value,
                                              int flag);

//----------------------------------------------------------------------------
//  0x6F288130 - slot 19, LoadFlags.  `retn 4`.  Mirrors SaveFlags: m_flags
//  read as a plain dword, m_field24/28/2C read as one SHandleTriple, then
//  m_field30 read as a plain dword (all three - CTask's own shape,
//  duplicated rather than reached through a base call, same as DumpState/
//  SaveFlags above), then the three point FloatMinis each read via
//  ReadCFloat and re-set through their own slot-0 notify.
//
//  All five reads (the two plain dwords and the three CFloats) share *one*
//  4-byte stack slot - the shipped code reuses the exact same scratch
//  region for `m_flags`, `m_field30` and every one of the three point
//  values, never a second local.  A same-sized `unsigned int` local plus a
//  same-TU `CFloat` local at a different type does not always coalesce to
//  that one slot at this toolchain (confirmed: it costs one extra
//  `push`/`pop` pair when they are two separate locals, even non-
//  overlapping and even after the helper that declared the CFloat one gets
//  inlined) - so LoadFloatMini takes its scratch buffer from the caller
//  instead of declaring its own, and every one of the five reads reuses
//  `value`'s one slot, matching the dump's own single-slot reuse exactly.
//----------------------------------------------------------------------------
static void LoadFloatMini(FloatMini* field, CDataStore* store, CFloat* scratch)
{
    ReadCFloat(store, scratch);
    ((FloatMiniNotifyFn)(*(void***)field)[0])(field, scratch, 0);
}

void CTaskPoint::LoadFlags(CDataStore* store)
{
    unsigned int value;

    store->ReadDword(&value);
    m_flags = value;

    ReadHandleTriple(store, reinterpret_cast<SHandleTriple*>(&m_field24));

    store->ReadDword(&value);
    m_field30 = value;

    CFloat* scratch = reinterpret_cast<CFloat*>(&value);
    LoadFloatMini(&m_pointX, store, scratch);
    LoadFloatMini(&m_pointY, store, scratch);
    LoadFloatMini(&m_pointZ, store, scratch);
}
