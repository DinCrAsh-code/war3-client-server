//============================================================================
//  CTaskPointAndTarget's seven serialisation-family vtable overrides -
//  GetRecordVersion, Save, Load, DumpState, SaveFlags, LoadFlags - all
//  clustered at 0x6F2883xx-0x6F2886xx, one module, one TU.  Like CTaskPoint/
//  CTaskTarget (and unlike CTaskParam/CTaskRotate/CTaskPoint2), this class
//  re-overrides Save/Load too, since it has real per-instance state (the
//  m_target handle) that needs its own round-trip on top of what
//  CTaskPoint::Save/CTask::Load already do for the inherited fields.
//============================================================================
#include "ctaskpointandtarget.h"
#include "../Net/cdatastorescratch.h"
#include "../Net/cdatastore.h"
#include "../Agent/handletriple.h"
#include "../Agent/presenceref.h"
#include "../Game/game.h"
#include "../Storm/textsink.h"

//  0x6F2871A0 - taskpoint_save.cpp's own LoadMovementRequestField, not
//  reconstructed this session - see that file's own comment.  `store` in
//  ecx, `&fieldAddr` in edx - fastcall, store as `this`.
CDataStore* __fastcall LoadMovementRequestField(CDataStore* store,
                                                 void* fieldAddr);  // 0x6F2871A0, not reconstructed

//----------------------------------------------------------------------------
//  0x6F2883C0 - slot 10, GetRecordVersion.  `retn 4`.  Same shape as
//  CTaskPoint's/CTaskTarget's own: a plain base-class call (not virtual
//  dispatch - straight to CAgent::GetRecordVersion, skipping every
//  intermediate override) plus this class's own constant.
//----------------------------------------------------------------------------
int CTaskPointAndTarget::GetRecordVersion(int context)
{
    return CAgent::GetRecordVersion(context) + 7;
}

//----------------------------------------------------------------------------
//  0x6F288650 - slot 14, Save.  `retn 4`.  CTaskPoint::Save(store) (the
//  real override, chained through - unlike Load below) first, then this
//  class's own m_target: if it resolves through LookupHandle, write its
//  real {m_handle, m_typeTag} pair; otherwise write {-1, -1}.  Same shape
//  CTaskPoint::Save already establishes for its own m_moveRequest.
//----------------------------------------------------------------------------
void CTaskPointAndTarget::Save(CDataStoreScratch* store)
{
    CTaskPoint::Save(store);

    //  m_target inherits m_handle/m_typeTag from CPresenceRef -
    //  MovementRequest itself adds nothing else this call needs.
    MovementRequest* ref = m_target;

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
//  0x6F2886B0 - slot 15, Load.  `retn 4`.  Unlike Save above, this one does
//  *not* chain through CTaskPoint::Load (0x6F288230) - it calls straight
//  into the same base Load CTaskPoint::Load itself calls first
//  (`CTask::Load`, byte-identical to CUnitRefList::Load, task.h's own
//  note), confirmed directly: the real call target here is 0x6F001A70, not
//  0x6F288230.  Then it loads m_moveRequest and m_target itself, in that
//  order - duplicating CTaskPoint::Load's own m_moveRequest call rather
//  than reaching it through a CTaskPoint::Load call, same "duplicate the
//  inherited field-by-field work instead of chaining through the immediate
//  override" shape DumpState/SaveFlags/LoadFlags use elsewhere in this
//  file (for those, the immediate override *is* chained - this is the one
//  exception, and the dump is unambiguous about it).
//----------------------------------------------------------------------------
void CTaskPointAndTarget::Load(CDataStore* store)
{
    CTask::Load(store);
    LoadMovementRequestField(store, &m_moveRequest);
    LoadMovementRequestField(store, &m_target);
}

//----------------------------------------------------------------------------
//  0x6F288410 - slot 17, DumpState.  `retn 4`.  CTaskPoint's own six fields
//  (m_flags/m_field24+28/m_field30/m_pointX/m_pointY/m_pointZ, duplicated
//  rather than reached through a base-class call - same shape the shipped
//  code takes for every DumpState in this family), then this class's own
//  new +0x50/+0x54 pair (m_field58, the triple's own "extra" dword, is not
//  dumped - same skip task.h's own m_field2C already documents).
//----------------------------------------------------------------------------
void CTaskPointAndTarget::DumpState(void* sink)
{
    reinterpret_cast<SIntMiniValue*>(&m_flags)->DumpTo(sink);
    reinterpret_cast<SRefMiniValue*>(&m_field24)->DumpTo(sink);
    reinterpret_cast<SIntMiniValue*>(&m_field30)->DumpTo(sink);
    m_pointX.DumpTo(sink);
    m_pointY.DumpTo(sink);
    m_pointZ.DumpTo(sink);
    reinterpret_cast<SRefMiniValue*>(&m_field50)->DumpTo(sink);
}

//----------------------------------------------------------------------------
//  0x6F2883D0 - slot 18, SaveFlags.  `retn 4`.  CTaskPoint::SaveFlags(store)
//  (the real override, chained through) first, then this class's own
//  +0x50/+0x54/+0x58 triple - two plain WriteDword calls then one
//  WriteDwordAlt2, same shape task.h's/tasktarget.h's own triples use.
//----------------------------------------------------------------------------
void CTaskPointAndTarget::SaveFlags(CDataStoreScratch* store)
{
    CTaskPoint::SaveFlags(store);

    store->WriteDword(m_field50);
    store->WriteDword(m_field54);
    store->WriteDwordAlt2(m_field58);
}

//----------------------------------------------------------------------------
//  0x6F288630 - slot 19, LoadFlags.  `retn 4`.  Mirrors SaveFlags:
//  CTaskPoint::LoadFlags(store) (chained through) first, then this class's
//  own +0x50/+0x54/+0x58 triple read as one SHandleTriple via
//  ReadHandleTriple - same shape task.h's/tasktarget.h's own triples use.
//----------------------------------------------------------------------------
void CTaskPointAndTarget::LoadFlags(CDataStore* store)
{
    CTaskPoint::LoadFlags(store);

    ReadHandleTriple(store, reinterpret_cast<SHandleTriple*>(&m_field50));
}
