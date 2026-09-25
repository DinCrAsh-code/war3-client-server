//============================================================================
//  CSelectable: the base CItem__Constructor (0x6F2B7520) builds before its
//  own body runs - a real, out-of-line constructor (sub_6F266C30, own
//  translation unit, selectable.cpp) rather than something inlined the way
//  CWidget's/CAgentWar3's trivial constructors are, because it in turn
//  calls a non-trivial base of its own (CAgent::CAgent(), reached through
//  two fully-inlined intermediate constructors).
//
//  **Derives from CWidget.**  The earlier reading here - CWidget and
//  CSelectable as siblings under CAgentWar3 - was wrong, and this
//  session's full sweep of all three vtables (docs/targets/
//  CWidget_CSelectable_CItem__vtables.md) is what settles it:
//
//    * every one of CWidget's 96 slots appears at the *same index* in
//      CSelectable's 107, with only seven of them overridden - which is
//      the single-inheritance override pattern and nothing else;
//    * sub_6F266C30's own field init is CWidget::CWidget()'s field init,
//      store for store: +0x24, +0x28, +0x2C/+0x2E, +0x30, +0x34, the -1
//      sentinel at +0x38 and the zero at +0x3C, with +0x40 left untouched
//      by both - CWidget's own trivial constructor inlined at its second
//      call site, exactly as it is at the first (0x6F2ACAE0);
//    * several slots CWidget, CSelectable and CItem all three share read
//      *CWidget's own fields* - vtable slot 94 (0x6F266C10) hands back
//      m_reserved40 at +0x40, slot 66 (0x6F2AC920) scales by
//      m_footprintType at +0x30, slot 17 (0x6F2AC770) dumps +0x20 and
//      +0x30 - which a class that did not derive from CWidget could not
//      inherit at all.
//
//  selectable.cpp's Reposition already had to cast `this` to CWidget* to
//  reach MoveTo, and its own comment called that "this function's own
//  real, unwritten base class"; it is written now.
//
//  +0x44/+0x48/+0x4C are read back, in a later session: CItem__Method_0x1A0
//  (0x6F2C7600, per user_knowledge.json's CItem vtable dump slot 104 -
//  the same slot CSelectable::Reposition above calls through and could not
//  identify at the time) treats +0x44/+0x48 as two independent "attachment
//  slot" indices - a `-1` in either means "no attachment there" - and
//  refreshes whichever one(s) are set with a Z-offset read from a config
//  key literally named "ImageWalkableZOffset" under section
//  "SelectionCircle", forwarding to two more Storm/Terrain-rendering gates
//  (0x6F741920/0x6F741970, same family as this repo's other
//  StormTerrain*Gate thunks - a selection-circle decal and a
//  walkable-terrain Z marker, not simulation state). +0x4C is a shared
//  flag both attachment refreshes branch on. This confirms +0x44/+0x48's
//  own initial value (g_unk6F932954, funcmap.py's DATA table) really is
//  the fixed constant `-1` in the shipped data segment - consistent with,
//  not contradicting, the "-1 = unset" sentinel this later reader checks
//  for. Not reconstructed itself this session (visual-attachment
//  bookkeeping, not item-placement logic, and its own two helpers reuse a
//  second, differently-addressed copy of GetConfigFloat's exact body -
//  configfloat.cpp - which would need its own translation unit to
//  reproduce faithfully); see docs/targets/CItem__Method_0x1A0.md for the
//  full trace. +0x50 is read back by slot 97 (0x6F2C7440,
//  selectable_vtable.cpp) - a rendering-side "selection visual" it
//  releases and clears - and is named for that now rather than left as
//  "zeroed".
//============================================================================
#ifndef SELECTABLE_H
#define SELECTABLE_H

#include "widget.h"
#include "CFloat.h"

//  Forward declarations for the slot signatures below - the vtable
//  block declares slots whose parameter types are defined further
//  down the include graph, and a pointer only needs the name.
class CDataStore;
class CDataStoreScratch;

class CSelectable : public CWidget
{
public:
    //====================================================================
    //  CSelectable's vtable contribution.  Declaration order *is* vtable order
    //  (C++'s own rule), so this block is sorted by slot index and has
    //  to stay that way - inserting one line in the wrong place renames
    //  every slot after it.  Slots 1-106; see
    //  docs/targets/CWidget_CSelectable_CItem__vtables.md.
    //====================================================================
    //  agentwar3_dtor.cpp, which is where it has to live to get
    //  ~CAgentWar3 inlined into it the way the shipped code does.
    //  slot 1 / +0x04 (0x6F2671B0)
    virtual CSelectable* DeleteSelf(int flags);
    //  changed.
    //  slot 7 / +0x1C (0x6F2C73F0)
    virtual unsigned int GetAgileTypeId() const;
    //  own two visual attachments, then a tail call into slot 97.
    //  slot 13 / +0x34 (0x6F2C7410)
    virtual void Deactivate();
    //  attachment-refresh flag.  See selectable_save.cpp.
    //  slot 14 / +0x38 (0x6F2C7760)
    virtual void Save(class CDataStoreScratch* store);
    //  attachment-refresh flag, Save's exact counterpart.  See
    //  selectable_load.cpp.
    //  slot 15 / +0x3C (0x6F2C7790)
    virtual void Load(class CDataStore* store);
    //  slot 22 / +0x58 (0x6F2C73A0)
    virtual const char* GetClassName();
    //  config value CWidget::Method_0x108 scales by the footprint, but raw
    //  and undoubled.
    //  slot 66 / +0x108 (0x6F2C74C0)
    virtual float Method_0x108();
    //  0x6F266C80 - CItem's own vtable[0x180] (index 96), per
    //  user_knowledge.json's CItem vtable dump - and per the same source,
    //  the identical function at CSelectable's and CDestructable's own
    //  vtable[0x180] slot too: none of the three override it, so this is
    //  the shared base implementation, not a CItem-specific one, and it
    //  belongs here rather than in item.h/item.cpp.  See selectable.cpp
    //  for the body (CItem__Method_0x180_0x6F266C80_calltree_asm.md).
    //
    //  Forwards nine of its own ten stack arguments straight through to
    //  CWidget::MoveTo (widget.h/widget.cpp) - a direct, non-virtual call
    //  (`call sub_6F2AC220`, `this` unchanged), which is only possible
    //  because this function's own real, unwritten base class is CWidget
    //  itself (this repo's own class model does not thread that
    //  connection - CSelectable derives CAgentWar3 directly per this
    //  file's own header comment - so the direct call is reproduced here
    //  by casting rather than by inheritance) - with one argument replaced
    //  by a literal null `facing` (MoveTo's own ninth parameter, "ask the
    //  handle-bearing sub-object for its own facing"), then calls whatever
    //  sits at this object's own vtable[0x1A0] (index 104, 0x6F278D40 per
    //  user_knowledge.json - not in this dump, not reconstructed) with no
    //  arguments beyond `this`, and returns that call's own return value.
    //
    //  This directly answers the question JASS_SetItemPosition_callee1's
    //  own writeup (jasssetitemposition.cpp) left open: CItem's
    //  vtable[0x180] - which callee1 calls with a freshly-copied target
    //  position and its own caller flag - is *not* CWidget::MoveTo itself,
    //  but this thin dispatcher, which immediately turns around and calls
    //  MoveTo with that same position.  So yes: JASS_SetItemPosition does
    //  end up moving the item through CWidget::MoveTo, just one hop
    //  further down than callee1's own vtable[0x180] call site made it
    //  look - see docs/targets/JASS_SetItemPosition.md.
    //  slot 96 / +0x180 (0x6F266C80)
    virtual int Reposition(CFloat* posX, CFloat* posY, int forward, int a4, int a5, int a6, int a7, int noDelta, int a10, int a11);
    //  slot 97 / +0x184 (0x6F2C7440)
    virtual void ReleaseSelectionVisual();
    //  always-zero answers, in CWidget's own 0x6F266xxx module rather than
    //  CSelectable's 0x6F2C7xxx one.
    //  slot 98 / +0x188 (0x6F266CD0)
    virtual int Method_0x188();
    //  slot 99 / +0x18C (0x6F266CE0)
    virtual int Method_0x18C();
    //  +0x00-+0x43: CAgent / CAgentWar3 / CWidget (base classes above).
    //  Everything from +0x20 to +0x40 used to be re-declared here, on the
    //  assumption that CSelectable was CWidget's sibling under CAgentWar3
    //  rather than its child - see this file's header comment for what
    //  changed that.  The offsets below are unchanged; the fields are the
    //  same fields, just no longer declared twice.  The old
    //  `m_reserved40[4]` "real gap" is CWidget::m_reserved40, which neither
    //  constructor writes.
    //
    //  The "pending" bit 0 that ClearPendingAndNotify clears is a bit of
    //  CAgentWar3::m_flags (agentwar3.h), whose SetPending() slot sets it.
    //  against slot 101's circle.  See selectable_slot100.cpp.
    //  slot 100 / +0x190 (0x6F2C74E0)
    virtual void RefreshSelectionHighlight(int wantHighlight, int visualArg);
    //  is missing, light it, and optionally run slot 103.  See
    //  selectable_attachcreate.cpp.
    //  slot 101 / +0x194 (0x6F2C7950)
    virtual void EnsureSelectionCircle(int wantUi, int runSlot103, int wantCircle, int unused3, int unused4);
    //  slot 102 / +0x198 (0x6F266CF0, nullsub_1201)
    virtual void RefreshSelectionState();
    //  gate (configgates.cpp).
    //  slot 103 / +0x19C (0x6F2C7750)
    virtual int Method_0x19C();
    //  at this object's current world position.  See
    //  selectable_attachrefresh.cpp.
    //  slot 104 / +0x1A0 (0x6F2C7600)
    virtual void RefreshAttachmentPlacement();
    //  attachment slots, as a two-float pair.
    //  slot 105 / +0x1A4 (0x6F2C76D0)
    virtual void SetAttachmentScale(float scale);
    //  slot 106 / +0x1A8 (0x6F2C7720)
    virtual void ShowAttachments();

    //  0x6F266C30 - see selectable.cpp.
    CSelectable();


    //====================================================================
    //  CSelectable's own vtable slots, from this session's full sweep.  See
    //  docs/targets/CWidget_CSelectable_CItem__vtables.md; bodies in
    //  selectable_vtable.cpp unless noted.  Same naming convention as
    //  widget.h: a real name where the body establishes one, a positional
    //  Method_0xNN otherwise.
    //====================================================================















    //  0x6F2C7460 - not a vtable slot: the selection-visual helper slot 100
    //  drives.  See selectable_selectionvisual.cpp.
    void UpdateSelectionVisual(int on, int arg);



    unsigned int   m_attachmentSlot1; // +0x44 - g_unk6F932954 (-1 = none); CItem__Method_0x1A0
    unsigned int   m_attachmentSlot2; // +0x48 - g_unk6F932954 (-1 = none); CItem__Method_0x1A0
    unsigned int   m_attachmentRefreshFlag; // +0x4C - zeroed; CItem__Method_0x1A0's shared branch
    //  +0x50 - zeroed by the constructor; slot 97 (0x6F2C7440) is the
    //  first thing to read it back: a nullable pointer to a rendering-side
    //  selection visual, released and cleared there.
    void*          m_pSelectionVisual;   // +0x50
};

#endif
