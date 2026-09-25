//============================================================================
//  CItem: the root of the CItem__Constructor dump (0x6F2B7520).
//
//  Single inheritance from CSelectable only - user_knowledge.json's own
//  CItem vtable dump confirms this: it overrides only some of
//  CSelectable's slots (1, 3, 22-23, 25, ...) and keeps the rest
//  identical, the ordinary single-base override pattern with no other
//  polymorphic base mixed in.  FloatMiniB and Position (floatmini.h/
//  position.h) are *members*, not further bases: the dump stores their
//  plain, un-adjusted vtables (`??_7FloatMini@@6B@`, `??_7Position@@6B@`)
//  at +0x54/+0x64, not a CItem-relative thunk vtable the way a genuine
//  second/third polymorphic base would need (multiple inheritance was
//  tried first and produces exactly that - three separate `??_7CItem@@6B
//  ...@` thunk vtables, none of which the dump has, and a re-stamp of all
//  three deferred to the very end of construction instead of interleaved
//  the way the dump shows).  Declaration order still matches the shipped
//  instruction order: CSelectable's base construction (a real, out-of-
//  line call) first, then m_floatB and m_position (both fully inlined)
//  right after CItem's own vtable store, then the two embedded
//  CAgentTimer members (each a real, out-of-line call) last.
//
//  `Position` (Widget/position.h) is the same shipped class as
//  Pathfinding/positiontrack.h's `PositionTrack` - see that file's own
//  header comment for the full evidence and the fix that corrected
//  Widget/position.h's own base-class chain (it now derives from
//  `PositionTrack` for its first 0x10 bytes instead of an invented
//  placeholder). Nothing in this file changed as a result - `m_position`'s
//  own trailing fields and every `Item/*.cpp` call site untouched.
//============================================================================
#ifndef ITEM_H
#define ITEM_H

#include "selectable.h"
#include "floatmini.h"
#include "position.h"
#include "agenttimer.h"
#include "timesyncbounds.h"
#include "fvec3.h"

//----------------------------------------------------------------------------
//  The placement descriptor CItem's slot 107 (item_place.cpp) is handed: a
//  whole SWidgetArtDescriptor (widget.h) followed by nine more words the
//  item path adds.  Only the ones slot 107 reads are named; +0x58..+0x78 is
//  its own block and nothing else in this codebase reaches into it.
//----------------------------------------------------------------------------
struct SItemPlacementDescriptor : public SWidgetArtDescriptor
{
    unsigned int m_field58;     // +0x58 -> Position's +0x74
    unsigned int m_field5C;     // +0x5C -> Position's +0x84, and m_flags bit
                                //          0x1000 *inverted*
    unsigned int m_field60;     // +0x60 -> m_flags bit 0x200
    unsigned int m_field64;     // +0x64 -> m_flags bit 0x40
    unsigned int m_field68;     // +0x68 -> m_flags bit 0x400
    unsigned int m_field6C;     // +0x6C -> m_flags bit 0x80
    unsigned int m_field70;     // +0x70 -> m_flags bit 0x800
    unsigned int m_field74;     // +0x74 -> m_flags bit 0x2000
    //  +0x78 - a four-character *string*, not a word: PackFourCCString
    //  (packfourcc.cpp) walks up to four bytes of it and stops at the first
    //  zero, which no integer would need.
    const char*  m_pTypeName;   // +0x78 -> Position's +0x88, packed
};

//  Forward declarations for the slot signatures below - the vtable
//  block declares slots whose parameter types are defined further
//  down the include graph, and a pointer only needs the name.
class CDataStore;
class CDataStoreScratch;
struct SAgentMessage;
struct SHandleWithType;
struct SWidgetArtDescriptor;

class CItem : public CSelectable
{
public:
    //====================================================================
    //  CItem's vtable contribution.  Declaration order *is* vtable order
    //  (C++'s own rule), so this block is sorted by slot index and has
    //  to stay that way - inserting one line in the wrong place renames
    //  every slot after it.  Slots 1-109; see
    //  docs/targets/CWidget_CSelectable_CItem__vtables.md.
    //====================================================================
    //  the real destructor it calls out of line (0x6F2B7780).  Both in
    //  item_dtor.cpp.
    //  slot 1 / +0x04 (0x6F2B7800)
    virtual CItem* DeleteSelf(int flags);
    //  vtables: cancel one of the two embedded timers depending on which of
    //  two message ids arrived.
    //  slot 3 / +0x0C (0x6F2B7DE0)
    virtual int Method_0x0C(const struct SAgentMessage* msg);
    //  0x6F2B5B10 - CItem's own override of CAgent's vtable[0x1C] (slot 7,
    //  agent.h). Returns the same constant GetItemTypeFourCC() does -
    //  matches CLAUDE.md's own house rule that this is the "real" type tag
    //  answer for anything already type-checked against 'item'
    //  (itemtypedassign.cpp).
    //  slot 7 / +0x1C (0x6F2B5B10)
    virtual unsigned int GetAgileTypeId() const;
    //  base answer plus one, with the object's Position told about it in
    //  between.
    //  slot 8 / +0x20 (0x6F2B5C00)
    virtual int Method_0x20(int context);
    //  slot 9 / +0x24 (0x6F2B6040)
    virtual int Method_0x24(int context);
    //  timers cancelled.
    //  slot 13 / +0x34 (0x6F2B5C30)
    virtual void Deactivate();
    //  CItem's own persistent members.  See item_save.cpp.
    //  slot 14 / +0x38 (0x6F2B69B0)
    virtual void Save(class CDataStoreScratch* store);
    //  four version-gated fields.  See item_load.cpp.
    //  slot 15 / +0x3C (0x6F2B7860)
    virtual void Load(class CDataStore* store);
    //  slot 16 / +0x40 (0x6F2B7B60)
    virtual void RefreshOwningPlayerColor();
    //  CItem's own printable members.  See item_dump.cpp.
    //  slot 17 / +0x44 (0x6F2B6070)
    virtual void DumpState(void* sink);
    //  slot 22 / +0x58 (0x6F2B5AC0)
    virtual const char* GetClassName();
    //  slot 23 / +0x5C (0x6F2B7660)
    virtual void Method_0x5C();
    //  so an item's owner lives in its Position record.
    //  slot 25 / +0x64 (0x6F2B7770)
    virtual unsigned int Method_0x64();
    //  slot 32 / +0x80 (0x6F2B6130)
    virtual void ClearPendingAndNotify();
    //  teardown an item does when it becomes pending.  See item_pending.cpp.
    //  slot 33 / +0x84 (0x6F2B7C20)
    virtual void SetPending();
    //  0x6F2B76B0 - CItem's own vtable[0xB8] (index 46): hand back
    //  &m_position reinterpreted as the handle-bearing sub-object every
    //  other reader of this slot (widget.cpp, jasssetitemposition.cpp)
    //  already assumed it would be, now confirmed by this function's own
    //  body (`lea eax,[ecx+64h]` - exactly m_position's own offset below).
    //  Not a formal C++ virtual, matching this repo's own established
    //  convention for these raw-offset-dispatched slots (widget.h) -
    //  reached only through `(GetHandleObjectFn)(*(void***)this)[0xB8/4]`,
    //  never through `this->GetHandleObject()`.
    //  slot 46 / +0xB8 (0x6F2B76B0)
    virtual SHandleWithType* GetHandleObject();
    //  CWidget's (g_flt6F932004 against g_flt6F931ED0).
    //  slot 47 / +0xBC (0x6F2B5DA0)
    virtual float Method_0xBC();
    //  slot 60 / +0xF0 (0x6F2B76A0)
    virtual int QueryVisible(int which);
    //  the item type's own tint modulated in.  See item_slot65.cpp.
    //  slot 65 / +0x104 (0x6F2B9BE0)
    virtual void RefreshTerrainTint();
    //  without the doubling.
    //  slot 66 / +0x108 (0x6F2B6100)
    virtual float Method_0x108();
    //  slot 69 / +0x114 (0x6F2B5C50)
    virtual int Method_0x114();
    //  to the FloatMiniB value at +0x54, publishing through its vtable and
    //  running the depletion tail on the downward crossing.  See
    //  item_valueset.cpp.
    //  slot 73 / +0x124 (0x6F2B89E0)
    virtual void SetLife(const CFloat* value);
    //  slot 74 / +0x128 (0x6F2B8A50)
    virtual CFloat AddLife(const CFloat* delta);
    //  CWidget answers with encoded zero, answered here out of m_floatB.
    //  slot 75 / +0x12C (0x6F2B76E0)
    virtual CFloat* GetLife(CFloat* out);
    //  slot 76 / +0x130 (0x6F2B76F0)
    virtual CFloat* GetMaxLife(CFloat* out);
    //  slot 77 / +0x134 (0x6F2B7700)
    virtual CFloat* GetMinLife(CFloat* out);
    //  for the pair slots 76/77 read back.  CWidget leaves both nullsub.
    //  slot 80 / +0x140 (0x6F2B7710)
    virtual void SetMinLife(const CFloat* value);
    //  slot 81 / +0x144 (0x6F2B7740)
    virtual void SetMaxLife(const CFloat* value);
    //  slot 86 / +0x158 (0x6F2B7690)
    virtual int GetPlacementMask();
    //  says 10 and 2.
    //  slot 88 / +0x160 (0x6F2B7670)
    virtual int Method_0x160();
    //  slot 89 / +0x164 (0x6F2B7680)
    virtual int Method_0x164();
    //  up to 1.0, push it at m_position, and fold slot 88's answer into
    //  m_position's own low-24-bit word.  See item_radius.cpp.
    //  slot 91 / +0x16C (0x6F2B5C60)
    virtual void RefreshPlacementRadius();
    //  write m_position's own m_notifyHandle.  CWidget's slot 92 is the
    //  fixed -1; its slot 93 is a nullsub.
    //  slot 92 / +0x170 (0x6F2B76C0)
    virtual int GetPositionNotifyHandle();
    //  slot 93 / +0x174 (0x6F2B76D0)
    virtual void SetPositionNotifyHandle(int value);
    //  item_typelookup.cpp.
    //  slot 95 / +0x17C (0x6F2BA0A0)
    virtual unsigned int Method_0x17C();
    //  See item_selected.cpp.
    //  slot 98 / +0x188 (0x6F2B6FF0)
    virtual int Method_0x188();
    //  and refresh the selection overlay if so.  See
    //  item_selectionrefresh.cpp.
    //  slot 102 / +0x198 (0x6F2B6F80)
    virtual void RefreshSelectionState();
    //  the item's own notify handle.
    //  slot 106 / +0x1A8 (0x6F2B5CE0)
    virtual void ShowAttachments();
    //  a placement descriptor.  See item_place.cpp.
    //  slot 107 / +0x1AC (0x6F2B7C60)
    virtual void PlaceFromDescriptor(unsigned int footprintType, SItemPlacementDescriptor* desc);
    //  m_position's own attachment-ref array *backwards* and turn every
    //  live entry on (109) or off (108).  See item_attachments.cpp.
    //  slot 108 / +0x1B0 (0x6F2B6450)
    virtual void SetAttachmentsOff(void* context, void* arg);
    //  slot 109 / +0x1B4 (0x6F2B63D0)
    virtual void SetAttachmentsOn(void* context, void* arg);

    //  0x6F2B7520 - see item.cpp.  Every member below does its own work;
    //  CItem's own body has nothing left to do.
    CItem();

    //  0x6F2B6AE0 - see jasssetitemposition.cpp.
    int JASS_SetItemPosition_callee1(const CFloat* pX, const CFloat* pY,
                                      int flag);



    //  0x6F2B5D00 - see item.cpp.
    void StoreAndNotifyHandle(int value);

    //  0x6F2B5CC0 - item_armdroptimer.cpp.  Arm m_timer2 to re-check this
    //  item's drop/pickup state after a fixed delay.  `retn 8`: two
    //  pointer stack arguments, only the first (the delay) actually read -
    //  the second is pushed and cleaned but never touched.
    void ArmDropTimer(const CFloat* delay, const CFloat* unused);



    //====================================================================
    //  The rest of CItem's own vtable slots, from this session's full sweep
    //  of all three vtables - see docs/targets/
    //  CWidget_CSelectable_CItem__vtables.md.  Bodies in item_vtable.cpp
    //  unless a comment says otherwise.  Same naming convention as
    //  widget.h/selectable.h.
    //====================================================================

    ~CItem();





















    //  0x6F2B8330 / 0x6F2B8270 - not vtable slots: the depletion tail both
    //  of those share, and the removal path it ends in.  See
    //  item_valuefloor.cpp / item_remove.cpp.
    void OnValueFloorReached();
    void RemoveFromWorld();










    FloatMiniB   m_floatB;    // +0x54
    Position     m_position;  // +0x64
    CAgentTimer  m_timer1;    // +0xD0
    CAgentTimer  m_timer2;    // +0xE4
};

#endif
