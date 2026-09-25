//============================================================================
//  CTaskTarget's six serialisation-family vtable overrides -
//  GetRecordVersion, Save, Load, DumpState, SaveFlags, LoadFlags - all
//  clustered at 0x6F2887xx-0x6F2889xx, one module, one TU.  Like CTaskPoint
//  (taskpoint_save.cpp) and unlike CTaskParam/CTaskRotate, CTaskTarget
//  re-overrides Save/Load too: it has real per-instance state (m_target)
//  that needs its own round-trip on top of what CAgentWar3::Save/Load
//  already do for the base fields.
//============================================================================
#include "tasktarget.h"
#include "../Net/cdatastorescratch.h"
#include "../Net/cdatastore.h"
#include "../Agent/handletriple.h"
#include "../Agent/presenceref.h"
#include "../Game/game.h"
#include "../Storm/textsink.h"

//  0x6F6EFE00/0x6F6EED50 - Net/cdatastore_writecfloat.cpp,
//  cdatastore_readcfloat.cpp.  Forward-declared locally rather than pulled
//  in through a shared header - same convention Item/item_save.cpp and
//  Task/taskpoint_save.cpp already use for these two.
CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store,
                                           const CFloat* value);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

//----------------------------------------------------------------------------
//  0x6F288780 - slot 10, GetRecordVersion.  `retn 4`.  Same shape as
//  CTask's/CTaskPoint's own: a plain base-class call (not virtual dispatch -
//  straight to CAgent::GetRecordVersion, skipping CTask's own override)
//  plus this class's own constant.
//----------------------------------------------------------------------------
int CTaskTarget::GetRecordVersion(int context)
{
    return CAgent::GetRecordVersion(context) + 5;
}

//----------------------------------------------------------------------------
//  0x6F2888C0 - slot 14, Save.  `retn 4`.  CTask::Save(store) (the
//  inherited-from-CAgentWar3 body, unmodified - see task.h) first, then
//  this class's own m_target: if it still resolves through LookupHandle,
//  write its real {m_handle, m_typeTag} pair; otherwise write {-1, -1}.
//  Exactly CTaskPoint::Save's own shape (taskpoint_save.cpp) one field over.
//----------------------------------------------------------------------------
void CTaskTarget::Save(CDataStoreScratch* store)
{
    CTask::Save(store);

    //  m_target is a MovementRequest*, but only its CPresenceRef base
    //  (m_handle/m_typeTag) is read here - reinterpret_cast straight to
    //  that base rather than pulling in MovementRequest's own
    //  (unreconstructed) definition, same shape CTaskPoint::Save uses for
    //  m_moveRequest.
    CPresenceRef* ref = reinterpret_cast<CPresenceRef*>(m_target);

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
//  0x6F288920 - slot 15, Load.  `retn 4`.  CTask::Load(store) first, then
//  hand &m_target to sub_6F2871A0 - not reconstructed this session (out of
//  scope: it is the "resolve/allocate a MovementRequest and bind it to the
//  handle just read" path, no simpler than ConstructCTaskTarget's own
//  SEH-framed callee, and genuinely MovementRequest's own construction
//  machinery rather than anything CTaskTarget itself does) - declared here
//  only so the real call site compiles and matches.  Same address, same
//  signature, same "declared, not defined" boundary CTaskPoint::Load's own
//  LoadMovementRequestField already establishes (taskpoint_save.cpp) - this
//  is that same shared leaf, reached from a second class now.  `store` in
//  ecx, `&m_target` in edx - fastcall, store as `this`.
//----------------------------------------------------------------------------
CDataStore* __fastcall LoadMovementRequestField(CDataStore* store,
                                                 void* fieldAddr);  // 0x6F2871A0, not reconstructed

void CTaskTarget::Load(CDataStore* store)
{
    CTask::Load(store);
    LoadMovementRequestField(store, &m_target);
}

//----------------------------------------------------------------------------
//  0x6F288810 - slot 17, DumpState.  `retn 4`.  CTask's own three fields
//  (m_flags/m_field24+28/m_field30, duplicated rather than reached through
//  a base-class call - same shape CTaskPoint::DumpState takes, one real
//  call per field rather than one call into CTask::DumpState), then the
//  second SHandleTriple (m_field34/38, dumped as a ref pair - m_field3C is
//  not separately dumped, same as m_field2C above it), then m_fieldWeight's
//  own FloatMini::DumpTo.
//----------------------------------------------------------------------------
void CTaskTarget::DumpState(void* sink)
{
    reinterpret_cast<SIntMiniValue*>(&m_flags)->DumpTo(sink);
    reinterpret_cast<SRefMiniValue*>(&m_field24)->DumpTo(sink);
    reinterpret_cast<SIntMiniValue*>(&m_field30)->DumpTo(sink);
    reinterpret_cast<SRefMiniValue*>(&m_field34)->DumpTo(sink);
    m_fieldWeight.DumpTo(sink);
}

//----------------------------------------------------------------------------
//  0x6F288790 - slot 18, SaveFlags.  `retn 4`.  CTask's own five fields
//  (+0x20..+0x30, duplicated the same way DumpState is - one real call per
//  field), then the second triple's three dwords (+0x34/+0x38 via
//  WriteDword, +0x3C via WriteDwordAlt2 - the same Alt2-on-the-"extra"-word
//  shape CTask's own +0x2C uses), then m_fieldWeight's own CFloat value
//  through WriteCFloat.  Copied to a local first: WriteCFloat takes a
//  `const CFloat*`, and passing `&m_fieldWeight.m_value` directly would
//  alias `this` across the call instead of the shipped code's own
//  read-into-local-then-address-of-local shape (the same reason
//  CTaskPoint::SaveFlags copies each of its own three FloatMinis to a local
//  first).
//----------------------------------------------------------------------------
void CTaskTarget::SaveFlags(CDataStoreScratch* store)
{
    store->WriteDword(m_flags);
    store->WriteDword(m_field24);
    store->WriteDword(m_field28);
    store->WriteDwordAlt2(m_field2C);
    store->WriteDword(m_field30);
    store->WriteDword(m_field34);
    store->WriteDword(m_field38);
    store->WriteDwordAlt2(m_field3C);

    CFloat weight = m_fieldWeight.m_value;
    WriteCFloat(store, &weight);
}

//  0x6F4770E0's own vtable slot 0 - see Math/floatmini.h's own note on
//  FloatMiniB::SetPair for why this is reached by raw offset rather than a
//  named virtual: it is a real "set value and notify" override, not the
//  placeholder destructor this reconstruction declares at that slot.  Same
//  typedef CTaskPoint::LoadFlags's own LoadFloatMini helper uses
//  (taskpoint_save.cpp) - inlined directly here rather than through a
//  shared static helper, since this class reads only one FloatMini (the
//  shipped body makes no separate call for it, unlike CTaskPoint's three).
typedef void (__thiscall *FloatMiniNotifyFn)(void* self, CFloat* value,
                                              int flag);

//----------------------------------------------------------------------------
//  0x6F288850 - slot 19, LoadFlags.  `retn 4`.  Mirrors SaveFlags: m_flags
//  read as a plain dword, m_field24/28/2C read as one SHandleTriple, then
//  m_field30 read as a plain dword (all three - CTask's own shape,
//  duplicated rather than reached through a base call, same as DumpState/
//  SaveFlags above), then the second SHandleTriple (m_field34/38/3C) read
//  the same way, then m_fieldWeight read via ReadCFloat and re-set through
//  its own slot-0 notify (flag 0, not CTaskPoint's own SetPair-style 1).
//----------------------------------------------------------------------------
void CTaskTarget::LoadFlags(CDataStore* store)
{
    unsigned int value;

    store->ReadDword(&value);
    m_flags = value;

    ReadHandleTriple(store, reinterpret_cast<SHandleTriple*>(&m_field24));

    store->ReadDword(&value);
    m_field30 = value;

    ReadHandleTriple(store, reinterpret_cast<SHandleTriple*>(&m_field34));

    //  The scratch CFloat is left raw (an `unsigned int` reinterpreted
    //  rather than a real `CFloat local;`) because CFloat's own default
    //  constructor zero-initialises m_bits - a real store the compiler
    //  cannot prove dead ahead of an extern call - and the shipped body has
    //  no such store: ReadCFloat's own pointer argument is the value's only
    //  write.
    FloatMini* weight = &m_fieldWeight;
    unsigned int localBits;
    ReadCFloat(store, reinterpret_cast<CFloat*>(&localBits));
    ((FloatMiniNotifyFn)(*(void***)weight)[0])(
        weight, reinterpret_cast<CFloat*>(&localBits), 0);
}
