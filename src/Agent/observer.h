//============================================================================
//  CObserver: a small TRefCnt-derived class the CWidget__Constructor dump
//  (0x6F2ACAE0) constructs twice inside CAgent::CAgent (0x6F001730) - once
//  as CAgent's own base subobject at offset 0, once as a plain member at
//  +0x14 whose refcount is then bumped to 1 (an "observer of itself" style
//  self-registration; nothing in this call tree explains what the +0x14
//  member is really for beyond that one increment) - and destructs once,
//  standalone, at 0x6F62A9D0.
//
//  Only the one extra field (+0x08, cleared by the constructor and read by
//  the destructor) is named here; nothing in this call tree gives it a real
//  type beyond "a pointer this destructor treats as an owning reference to
//  something with an ObserverEventReg-shaped list attached" (see
//  observereventreg.h).
//============================================================================
#ifndef OBSERVER_H
#define OBSERVER_H

#include "refcnt.h"

//----------------------------------------------------------------------------
//  The message record the observer slots below pass around: only the id at
//  +0x08 is ever read (by CObserver::Method_0x10, which forwards it to
//  Method_0x14, and by CItem's own slot 3 override, item_vtable3.cpp, which
//  switches on it).  Nothing in these call trees reads the first eight
//  bytes.
//----------------------------------------------------------------------------
struct SAgentMessage
{
    char         m_reserved00[8];
    unsigned int m_id;      // +0x08
    char         m_reservedC[0x10 - 0x0C];
    //  +0x10 - CCameraWar3::Update's (0x6F308E70) own second selector: a
    //  controller-slot index for the "8 shared" small-switch cases, and a
    //  second, larger message-subtype code for the big dispatch below it.
    int          m_field10;
    char         m_reserved14[0x2C - 0x14];
    //  +0x2C - CCameraWar3::Update's own 0x400500CD case: a raw count it
    //  divides by 9 (the shipped `imul 88888889h`/`sar 6` magic-number
    //  trick for signed division) to decide which way to step the
    //  target-distance preset.
    int          m_field2C;
};

class CObserver : public TRefCnt
{
public:
    //====================================================================
    //  CObserver's vtable contribution.  Declaration order *is* vtable order
    //  (C++'s own rule), so this block is sorted by slot index and has
    //  to stay that way - inserting one line in the wrong place renames
    //  every slot after it.  Slots 1-5; see
    //  docs/targets/CWidget_CSelectable_CItem__vtables.md.
    //====================================================================
    //  slot 1 / +0x04 (0x6F62AA80, sub_6F62AA80)
    virtual CObserver* DeleteSelf(int flags);
    //  against this observer's resource, creating the resource if it does
    //  not exist yet.  See observer_slots.cpp.
    //  slot 2 / +0x08 (0x6F62A9A0)
    virtual void Dispatch(unsigned int msg, unsigned int arg1, void* arg2);
    //  answer (agentwar3.h) and CItem overrides it with a real message
    //  handler (0x6F2B7DE0).
    //  slot 3 / +0x0C (0x6F0017A0)
    virtual int Method_0x0C(const struct SAgentMessage* msg);
    //  message itself to slot 5, tail-forwarding slot 5's own return value -
    //  see netcommand.h's CNetData::FireToObserverLists, which is the call
    //  site that first needed the return value read (netcommand_fire.cpp).
    //  slot 4 / +0x10 (0x6F629A90)
    virtual int Method_0x10(SAgentMessage* msg);
    //  a temporary reference, invoke it, and release this observer if that
    //  reference was the last one.
    //  slot 5 / +0x14 (0x6F62A7B0)
    virtual int Method_0x14(unsigned int msgId, SAgentMessage* msg);

    //  0x6F62A570 - post one event to this observer's own registration
    //  list.  Not a vtable slot: CWidget::PostAgentEvent
    //  (widget_postagentevent.cpp) tail-jumps straight into it, `this` in
    //  ecx and both arguments on the stack (`retn 8`).
    void PostEvent(unsigned int event, void* target);

    //  0x6F4269E0 / 0x6F426A20 - CUnit batch-15 (slot 16 closure).  Both are
    //  thin `retn 4` wrappers over PostEvent with a fixed event id baked in
    //  (`push eax / push <id> / call PostEvent`), `this` untouched in ecx.
    //  The two ids, 0x80264/0x80265, are the identical pair
    //  agentdaynight.cpp's own kEventBecameDay/kEventBecameNight use, but
    //  this call site reaches them through the player-slot table singleton
    //  (dword_6FAB65F4) rather than CAgentRegistrySingleton, and nothing in
    //  this call tree ties the two together - so they are named for what
    //  is certain (a fixed-id PostEvent wrapper) rather than assumed to be
    //  the same broadcast.
    void PostEvent80264(void* target);
    void PostEvent80265(void* target);

    //  0x6F629A70 - construct empty: no owned resource yet.
    CObserver();

    //  0x6F62A9D0 - if a resource is owned, release its event-registration
    //  list (ObserverEventReg's own cleanup, sub_6F62A2A0 via the
    //  sub_6F62A5C0 thunk) and free the resource itself back to the pool it
    //  came from.  See observer.cpp for why the SEH frame around this one
    //  does not reproduce.
    ~CObserver();

    //====================================================================
    //  Vtable slots 2-5.  These four used to be declared on CAgent
    //  (Dispatch / Reserved0C / Reserved10 / Reserved14) as slot-holding
    //  placeholders; this session's sweep of the CWidget/CSelectable/CItem
    //  vtables found all four implementations, and three of them sit in
    //  *this* class's own 0x6F629xxx/0x6F62Axxx module and touch nothing
    //  but m_resource and m_refcount.  Moving the declarations down here
    //  is slot-neutral - CObserver contributes exactly four virtuals after
    //  TRefCnt's two either way, so CAgent's own GetAgileTypeId stays at 7
    //  - and it puts each body on the class that owns its fields.
    //====================================================================





    void*   m_resource;   // +0x08 - cleared by the constructor, released by the destructor

    //  0x6F629F70 - not a vtable slot: "give me the resource, creating one
    //  if `create` is set".  Its own translation unit
    //  (observer_resource.cpp), because Dispatch above reaches it with a
    //  real call.
    void* EnsureResource(int create);
};

#endif
