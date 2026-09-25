//============================================================================
//  CAgentTickRelay - the scheduler-heap node callback object at node+0x18
//  (scheduler.h) that resolves an agent handle and relays a tick event to
//  it.  CMissileThunderBolt impact pipeline, frames [8]/[7]
//  (docs/targets/cmissilethunderbolt_impact_pipeline.md).
//
//  Named to avoid collision with the *different*, already-reconstructed
//  `CAgentTimer` (agenttimer.h - a small CItem-embedded queue-cancel
//  wrapper, ctor 0x6F0418E0): that class's own `m_queue` is a pointer to a
//  scheduler.h node, the same node this class is *entered from* (its own
//  vtable slot 0x48, invoked by scheduler.cpp's InvokeSchedulerNodeCallback)
//  - the two are related but distinct objects in this closure, and the
//  ground-truth pipeline notes' own "CAgentTimer" label for this one
//  collides with the pipeline's prior, unrelated use of that name.
//  vtable at 0x6F8784AC per those notes (CAgentTimerExtended's own sibling
//  at 0x6F8784D8 is not reached by anything in this closure).
//
//  +0x0C/+0x10 read as a second {value,value} pair copied verbatim into the
//  event context this class builds (FireEvent below) - not yet confirmed
//  against any other call site, so left as untyped words rather than named
//  for a guessed purpose.  +0x1C/+0x20 are a matched pair (ReleasePendingA/
//  ReleasePendingB have identical bodies against the two offsets), each
//  looking like an independently-armed pending-node reference that gets
//  flagged (bit 0x10000 on the referenced node) and cleared on release -
//  named for that shape, not for a confirmed purpose.  +0x24/+0x28 are the
//  documented agent {handle,typeTag} pair.  +0x30 is a flag word: bit 0
//  gates the handle resolve, bit 0x40000 is read (not yet a confirmed
//  meaning) by the sibling reload path, bit 2 is toggled by Arm().
//============================================================================
#ifndef AGENTTICKRELAY_H
#define AGENTTICKRELAY_H

#include "game.h"

class CAgent;

//  The 11-dword event context CAgentTickRelay::FireEvent builds and hands
//  down through the agent's own +0x30 event-router sub-object
//  (agenteventrouter.h) and on into the missile dispatch switch.
struct SAgentTickEventCtx
{
    void*        field0;      // +0x00 <- relay->m_field0C
    void*        field4;      // +0x04 <- relay->m_field10
    unsigned int eventCode;   // +0x08
    CAgent*      agent;       // +0x0C
    void*        field10;     // +0x10 = 0
    void*        field14;     // +0x14 = 0
    void*        field18;     // +0x18 = 0
    void*        field1C;     // +0x1C = 0
    void*        field20;     // +0x20 = 0
    int          field24;     // +0x24 = -1
    int          field28;     // +0x28 = -1
};

class CAgentTickRelay
{
public:
    void*           m_vtable;          // +0x00
    unsigned char   m_reserved04[8];   // +0x04 (owner CAgent* per the pipeline notes, not read here)
    void*           m_field0C;         // +0x0C
    void*           m_field10;         // +0x10
    unsigned char   m_reserved14[8];   // +0x14
    void*           m_pendingA;        // +0x1C
    void*           m_pendingB;        // +0x20
    unsigned int    m_agentHandle;     // +0x24
    int             m_agentTypeTag;    // +0x28
    unsigned char   m_reserved2C[4];   // +0x2C
    unsigned int    m_flags;           // +0x30

    //  0x6F4AB150 - resolve m_agentHandle/m_agentTypeTag through the global
    //  handle table, gated on m_flags bit 0.  The stack argument is pushed
    //  as a literal 0 by every call site in this closure and never read.
    CAgent* __thiscall ResolveAgentHandle(int /*unused*/);

    //  0x6F496CA0 - resolve the agent, build an 11-dword event context
    //  around it and `eventCode`, then relay through the resolved agent's
    //  own +0x30 sub-object vtable slot 0x20 (only once that sub-object is
    //  non-null; otherwise the resolve's own result is returned unchanged).
    void* __thiscall FireEvent(unsigned int eventCode);

    //  0x6F4AA510 / 0x6F4AA530 - identical shape against m_pendingA/
    //  m_pendingB: if the pending reference is set, flag its target
    //  (bit 0x10000 at +0x10) and clear the reference.
    void __thiscall ReleasePendingA();
    void __thiscall ReleasePendingB();

    //  0x6F4AA560 - stack args (arm, releaseA, releaseB): conditionally
    //  release each pending reference when arm is set, then set or clear
    //  m_flags bit 1 depending on arm.
    void __thiscall Arm(int arm, int releaseA, int releaseB);

    //  0x6F497300 - the scheduler-heap node callback itself (node+0x18 in
    //  the pipeline's own table, vtable slot 0x48 - scheduler.cpp's
    //  InvokeSchedulerNodeCallback).  `node` is the popped heap node.  If
    //  `node` is the one this relay itself is still waiting on (m_pendingA)
    //  it reloads/re-arms; otherwise it fires the completion event carried
    //  in `node`'s own +0x1C word.
    void __thiscall Tick(void* node);

    //  0x6F496980 - the reload/re-arm path Tick takes when `node` is the
    //  relay's own currently-armed node.  Both of its own real callees are
    //  reconstructed now: `sub_6F493A40` is `CPathTrace::SetVelocity`
    //  (Pathfinding/pathtracesetvelocity.cpp), and `sub_6F4AA6E0` is
    //  `FireGlobalAgentEvent` below.
    void __thiscall Reload();

    //  0x6F4AA6E0 - fires a second, FourCC-keyed observer event through a
    //  caller-supplied `slot` pointer, arming `m_pendingB` with whatever the
    //  registration call below hands back (the same "flag the old target,
    //  then replace the reference" shape ReleasePendingA/B's own doc
    //  comment already gives for this field pair - this is the counterpart
    //  that ARMS m_pendingB rather than releasing it).  Own translation
    //  unit (agenttickrelay_fireglobalevent.cpp), deliberately separate
    //  from Reload above - with both in one TU, `/Ob2` inlines this whole
    //  body into Reload's own and the real `call` is lost (measured).
    //  `sub_6F4AA630` (declared there, not reconstructed: its own body
    //  bottoms out in `sub_6F4AA5A0`, the same "genuinely new allocator
    //  pattern" leaf `prbehaviorloadhelpers.cpp` already documents as out
    //  of scope for this cluster) does the actual registration against the
    //  subsystem `SelectSubsystem` picks.
    void __thiscall FireGlobalAgentEvent(void* slot, unsigned int fourCC);
};

#endif
