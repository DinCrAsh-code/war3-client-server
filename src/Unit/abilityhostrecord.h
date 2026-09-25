//============================================================================
//  SAbilityHostRecord - the class CUnit::NotifyAbilityHostChanged
//  (0x6F2E52F0, unit_abilityhostnotify.cpp) resolves through the owning
//  player's own SPlayerScore::m_ref2D4 handle-ref pair
//  (Player/playerscore.h - "not otherwise identified" there) and then
//  calls back with `Notify_6F2DBB90(this)`, i.e. sub_6F2DBB90 below.
//
//  Not otherwise named anywhere in this codebase; every member here is
//  reached only from that one closure (createunit-G2's own dump,
//  asm/sub_6F2DBB90_0x6F2DBB90_calltree_asm.md), so field names stay
//  address-derived/mechanical rather than guessed, per this repo's own
//  rule for a type nothing has independently identified.  Only the offsets
//  the dump actually reads are named; everything else is a reserved gap.
//============================================================================
#ifndef ABILITYHOSTRECORD_H
#define ABILITYHOSTRECORD_H

#include "itemhandleresolve.h"   // SOptionalHandleRef, QueryHandleField0x54

class CUnit;
class CAgent;

struct SAbilityHostRecord
{
    //  0x6F2D6DA0 - if the world singleton's own +0x3E0 flag is set,
    //  force-set bit 20 of +0x2D0 and report true; otherwise report
    //  whether bit 20 is currently clear.  Called first thing by
    //  0x6F2D8190 (unit_ability_notifyfamily.cpp's own worklist slice) and
    //  by sub_6F2DBB90 itself.
    int TestOrArmFlagBit20();

    //  0x6F2CFE10 - conditionally register `this` as an observer of
    //  `target` through up to three different registration entry points -
    //  CMissileThunderBoltRefs::SetObserverRegistration (0x6F2AB2D0,
    //  missile_thunderbolt.h), its sibling SEventNotifyOrForwardHost::
    //  NotifyOrForwardEvent (0x6F2AB3E0, a standalone leaf reconstructed by
    //  a different parallel agent - Misc/misc_notifyorforward_event.cpp,
    //  not a CMissileThunderBoltRefs member despite living at a neighbouring
    //  address), and CWidget::PostAgentEvent2 (0x6F26EF10, widget.h) - one
    //  call per boolean, same (this, add) argument pair forwarded to each.
    //  `target`'s real type is whichever of those three the caller means it
    //  to be; modelled as `void*` and cast per call.
    void NotifyObserverRegistrations(void* target, int add,
                                      int doThunderBolt, int doAlt,
                                      int doWidgetEvent);

    //  0x6F2DBB90 - createunit-G2's own dump root.  See
    //  cunit_abilityhostnotifytarget.cpp.
    void NotifyUnitAbilityHostChanged(CUnit* unit);

    //  0x6F2CFEB0 (`retn 0`) - resolve the +0x2AC pair, or null.
    //  abilityhostrecord_handlerefs.cpp.
    void* GetField0x54At0x2AC();
    //  0x6F2CFED0 (`retn 0`) - same shape, +0x2B8.
    void* GetField0x54At0x2B8();
    //  0x6F2D6500 (`retn 0`) - resolve the +0x2C4 pair and hand back the
    //  resolved object only if it type-checks as a CUnit-derived agent
    //  (SCheckedUnitSlot, agenttypedslots.h) - the temporary's own
    //  destructor releases the extra reference before returning, so this
    //  is a type *test*, not a borrow.
    CAgent* GetUnitIfResolvedAt0x2C4();
    //  0x6F2D30F0 (`retn 4`) - resolve the +0x254 pair; if it resolves and
    //  m_field40 is set, notify the resolved object through the
    //  not-otherwise-identified 0x6F2DE1D0 leaf.  abilityhostrecord_notify2d30f0.cpp.
    void* NotifyResolvedAt0x254(void* arg0);
    //  0x6F2D5330 (`retn 4`) - a fixed-capacity (4), unordered,
    //  no-duplicates set at +0x1B8: if `value` is already one of the four
    //  slots, do nothing; otherwise store it in the first slot still
    //  holding the -1 sentinel (silently does nothing if all four are
    //  live).  abilityhostrecord_addunique1b8.cpp.
    void AddUniqueTo1B8Set(unsigned int value);

    //  0x6F2DF280 (`retn 4`) - FindNearestReachableIndex(unit, 0)
    //  (indexedrecordarray.h's own g_indexedRecordArray_6FAB4E90 walk,
    //  abilityhostrecord_findnearestslot.cpp) then AddUniqueTo1B8Set of
    //  whatever index it returns.  abilityhostrecord_addnearest1b8.cpp.
    void AddNearestTo1B8Set(CUnit* unit);

    //  0x6F2E5060 (`retn 8`) - the general form of AddNearestTo1B8Set:
    //  find the nearest reachable index and, if one resolved, notify its
    //  own indexed-array entry through the out-of-scope 0x6F2E3790 (only
    //  when `candidate` is set) before adding it; if none resolved, run
    //  one of two out-of-scope side effects instead
    //  (`candidate` set: 0x6F2E4FC0; clear: 0x6F2E4F00(fallback,
    //  fallback+4)) and add the current g_indexedRecordCount_6FAB4E74.
    //  abilityhostrecord_addnearestorfallback1b8.cpp.
    void AddNearestOrFallbackTo1B8Set(void* candidate, const void* fallback);

    //  0x6F2D8190 (`retn 0`) - guarded by TestOrArmFlagBit20(); walk the
    //  owner chain from m_ref28 and SubmitOrder_0xD0006(1) on every owner
    //  that isn't excluded.  abilityhostrecord_submitorderownerchain.cpp.
    void SubmitOrderAcrossOwnerChain();

    //  0x6F2DB850 (`retn 4`) - read `unit`'s own path position through its
    //  virtual GetHandleObject() (cast to CPathRef - the same 16-byte
    //  handle record under a second name), copy that CWorldVec3 into a
    //  second local next to it, and hand this record, `unit`'s own +0x30
    //  rawcode field and (a pointer into) that copy to the out-of-scope
    //  0x6F2DAA40 - its own `__except_handler4`-shaped SEH frame, kept a
    //  redirect for the same reason 0x6F2DBB90 itself is (see
    //  docs/msvc-vc8-idioms.md's "An `__except_handler4`-shaped frame this
    //  toolchain cannot reproduce").  The result is forwarded into
    //  NotifyPositionEvent (0x6F2E3B10) and returned.
    //  abilityhostrecord_positionnotify.cpp.
    void* ResolveAndNotifyPosition(CUnit* unit);

    //  0x6F2DB8C0 (`retn 4`) - guard in front of ResolveAndNotifyPosition:
    //  skip it whenever `unit` is already known invisible (+0x20 &
    //  0xD8000), whenever QueryVisibleImpl(1) says so, whenever the
    //  out-of-scope 0x6F03FDC0 probe says so, or whenever bit 4 of +0x248
    //  is set.  abilityhostrecord_positionnotify.cpp.
    void NotifyPositionIfVisible(CUnit* unit);

    //  0x6F2E3B10 (`retn 4`) - tell `unit` about this record both ways
    //  (unit->NotifyOrForwardEvent(this, 1) and
    //  unit->PostAgentEvent2(this, 1) - the receiver/target roles swapped
    //  from NotifyObserverRegistrations' own shape above), resolve the new
    //  +0x38 handle-ref pair chained off `unit` (ResolveChained,
    //  handlereref_resolve.cpp), bump the +0x44 refcount-shaped counter,
    //  resolve `unit`'s own m_ref304 (CUnit::ResolveRef304) and, if that
    //  resolved owner's own +0x30 pair also resolves, forward `unit`
    //  through the out-of-scope 0x6F2EC850; then unconditionally call the
    //  out-of-scope 0x6F2E61B0(0) on that owner and chain-resolve its own
    //  +0x3C pair against `this`.  abilityhostrecord_positionnotify.cpp.
    void NotifyPositionEvent(CUnit* unit);

    char           m_reserved00[0x28];
    SOptionalHandleRef m_ref28;    // +0x28/+0x2C
    char           m_reserved30[0x38 - 0x30];
    SOptionalHandleRef m_ref38;    // +0x38/+0x3C
    void*          m_field40;      // +0x40
    unsigned int   m_counter44;    // +0x44 - bumped by NotifyPositionEvent
    char           m_reserved48[0x1B8 - 0x48];
    unsigned int   m_set1B8[4];    // +0x1B8..+0x1C4
    char           m_reserved1C8[0x254 - 0x1C8];
    SOptionalHandleRef m_ref254;   // +0x254/+0x258
    char           m_reserved25C[0x2AC - 0x25C];
    SOptionalHandleRef m_ref2AC;   // +0x2AC/+0x2B0
    char           m_reserved2B4[0x2B8 - 0x2B4];
    SOptionalHandleRef m_ref2B8;   // +0x2B8/+0x2BC
    char           m_reserved2C0[0x2C4 - 0x2C0];
    SOptionalHandleRef m_ref2C4;   // +0x2C4/+0x2C8
    char           m_reserved2CC[0x2D0 - 0x2CC];
    unsigned int   m_flags2D0;     // +0x2D0
};

#endif
