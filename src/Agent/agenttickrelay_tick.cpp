//============================================================================
//  0x6F496CA0 / 0x6F496980 / 0x6F497300 - CAgentTickRelay::FireEvent/
//  Reload/Tick (see agenttickrelay.h).  Own TU, separate from
//  ResolveAgentHandle/Arm/ReleasePendingA/B: this file's own functions
//  reach those through real `call` instructions in the target listing.
//============================================================================
#include "agenttickrelay.h"
#include "agent.h"
#include "CFloat.h"
#include "game.h"

//----------------------------------------------------------------------------
//  0x6F496CA0 - resolve, build the 11-dword event context, relay through
//  the resolved agent's own +0x30 sub-object.
//----------------------------------------------------------------------------
typedef void* (__thiscall *AgentSub30FireFn)(void* self, SAgentTickEventCtx* ctx);

void* __thiscall CAgentTickRelay::FireEvent(unsigned int eventCode)
{
    CAgent* agent = ResolveAgentHandle(0);
    if (!agent)
        return 0;

    SAgentTickEventCtx ctx;
    ctx.field0 = m_field0C;
    ctx.field4 = m_field10;
    ctx.eventCode = eventCode;
    ctx.agent = agent;
    ctx.field10 = 0;
    ctx.field14 = 0;
    ctx.field18 = 0;
    ctx.field1C = 0;
    ctx.field20 = 0;
    //  High half first: the shipped code stores var_4 (0x6F496CE3) before
    //  var_8 (0x6F496CE7), i.e. field28 before field24.
    ctx.field28 = -1;
    ctx.field24 = -1;

    void* sub = *(void**)((char*)agent + 0x30);
    if (!sub)
        return agent;

    void* vtable = *(void**)sub;
    AgentSub30FireFn fn = *(AgentSub30FireFn*)((char*)vtable + 0x20);
    return fn(sub, &ctx);
}

//----------------------------------------------------------------------------
//  0x6F493A40 is reconstructed now - CPathTrace::SetVelocity,
//  Pathfinding/pathtracesetvelocity.cpp - and the naked redirect that stood
//  in for it here is gone with it.  What Reload does to the object it
//  resolves is set its velocity to zero and ask for the change to be
//  notified, which is what a relay reload does to a unit that has arrived;
//  the resolved handle really is a CPathTrace, because 0x6F493A40 hands the
//  same receiver to CPathTrace::PublishVelocityDelta unaltered.
//
//  0x6F4AA6E0 (CAgentTickRelay::FireGlobalAgentEvent) is reconstructed too,
//  but deliberately kept in its own TU (agenttickrelay_fireglobalevent.cpp)
//  rather than here alongside its only caller: with both in one file and
//  the callee's whole body visible, `/Ob2` inlines it straight into Reload
//  below and the real `call` this dump's own tail chunk makes is lost -
//  see that file's own header comment.
//----------------------------------------------------------------------------
//  0x6F496980 - the reload/re-arm path Tick takes when the popped node is
//  the relay's own currently-armed one (m_pendingA).
//
//  The `m_pendingA == 0` case is an early `return`, not a fall-through, and
//  that is a behavioural fix, not a shape preference: the shipped epilogue
//  at 0x6F4969DD pops and returns outright, while only the far path at
//  0x6F4969D2 reaches the m_pendingB test.  That test lives at 0x6F496820,
//  which is a cold *chunk* of this same function rather than a callee - the
//  `mov ecx,esi / pop esi / add esp,8 / jmp loc_6F496820` tail is MSVC's
//  block placement, confirmed by `ida_query calltree_asm 0x6F496820`
//  answering with sub_6F496980 as the root and no 0x6F496820 function of
//  its own - so it is written inline here, not split into a TU of its own.
//  A previous pass had it as a fall-through and fired the global event on
//  the null-m_pendingA path the shipped code skips entirely.
//----------------------------------------------------------------------------
void __thiscall CAgentTickRelay::Reload()
{
    if (!m_pendingA)
        return;

    ReleasePendingA();

    if (m_flags & 0x40000)
        Arm(1, 0, 0);

    CAgent* agent = ResolveAgentHandle(0);
    if (agent)
    {
        //  High word first: the shipped code zeroes var_4 before var_8
        //  (0x6F4969BD then 0x6F4969C5), which an aggregate initialiser
        //  emits the other way round.
        CGridVec2 stop(kCFloatNoInit);
        stop.m_y = CFloat(0);
        stop.m_x = CFloat(0);
        ((CPathTrace*)agent)->SetVelocity(&stop, 1);
    }

    if (!m_pendingB)
        FireGlobalAgentEvent((void*)&g_CFloatZero, 0x706A7E65);
}

//----------------------------------------------------------------------------
//  0x6F497300 - the scheduler-heap node callback (node+0x18 in the pipeline
//  table).  If `node` is the one this relay is itself still waiting on
//  (m_pendingA), reload; otherwise release the other pending reference
//  (m_pendingB) and fire the event carried in `node`'s own +0x1C word.
//----------------------------------------------------------------------------
void __thiscall CAgentTickRelay::Tick(void* node)
{
    if (node == m_pendingA)
    {
        Reload();
        return;
    }

    unsigned int eventCode = *(unsigned int*)((char*)node + 0x1C);
    ReleasePendingB();
    FireEvent(eventCode);
}
