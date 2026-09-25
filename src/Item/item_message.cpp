//============================================================================
//  0x6F2B7DE0 - CItem's vtable slot 3 (+0x0C), where every other class in
//  this family answers a flat 0 (0x6F0017A0, agentwar3_slots.cpp).
//
//  The one real message handler in these three vtables: two ids, one per
//  embedded timer.  0xD01C1 cancels the first timer (+0xD0) and follows up
//  with slot 23; 0xD01BE cancels the second (+0xE4) and does not.  The two
//  answer 1 and 0 respectively, and anything else answers 0.
//
//  Each arm tests the timer's own m_queue twice - once for "is anything
//  scheduled at all", once inside CAgentTimer's own
//  "has the queue already flagged this" guard - and the shipped code really
//  does reload the field for the second test, which is what an inlined
//  short-circuit `m_queue == 0 || (flag & 1)` gives.
//
//  Own translation unit: two real calls to CAgentTimer::Cancel plus a
//  vtable dispatch.
//============================================================================
#include "item.h"

//  slot 23 (+0x5C) - CAgent::Method_0x5C (agent_slot5c.cpp), reached
//  through the vtable because CItem overrides it with its own thunk.
typedef void (__thiscall *Slot0x5CFn)(void* self);

//  CAgentTimer::m_queue's own "already flagged" byte at +0x12 - agenttimer.h
//  gives the field no type beyond "a pointer to some other, unrecovered
//  object", so it is read by offset here too.
static inline int QueueAlreadyFlagged(const void* queue)
{
    return queue == 0 || (*((const unsigned char*)queue + 0x12) & 1) != 0;
}

int CItem::Method_0x0C(const SAgentMessage* msg)
{
    //  A real `switch`, not a chain of `if`s: the shipped code lowers the
    //  two ids as a *subtract* chain (`sub eax, 0D01BEh` / `jz` /
    //  `sub eax, 3` / `jnz`), which is what MSVC emits for a switch over
    //  two nearby constants and never for two independent comparisons -
    //  and it is the reason the first `m_queue == 0` test can be a
    //  `cmp [esi+0DCh], eax` against the register the subtraction has
    //  already left holding zero.
    switch (msg->m_id)
    {
    case 0xD01C1:
    {
        //  A named pointer to the timer, and the guard read through it:
        //  the shipped code computes `lea ecx,[esi+0D0h]` unconditionally
        //  above the branch and *re-loads* m_queue for the second test,
        //  which is what a second lvalue for the same object gives.
        //  Reaching through `this` both times lets MSVC prove the second
        //  `m_queue == 0` dead and fold the two tests into one.
        CAgentTimer* timer = &m_timer1;
        if (m_timer1.m_queue == 0)
            return 1;
        if (QueueAlreadyFlagged(timer->m_queue))
        {
            timer->Cancel();
            ((Slot0x5CFn)(*(void***)this)[0x5C / 4])(this);
        }
        return 1;
    }

    case 0xD01BE:
    {
        CAgentTimer* timer = &m_timer2;
        if (m_timer2.m_queue == 0)
            return 0;
        if (QueueAlreadyFlagged(timer->m_queue))
            timer->Cancel();
        return 0;
    }
    }
    return 0;
}
