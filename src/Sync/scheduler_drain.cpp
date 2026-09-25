//============================================================================
//  0x6F46EAB0 / 0x6F46E970 - the outer two frames of the CMissileThunderBolt
//  impact pipeline (docs/targets/cmissilethunderbolt_impact_pipeline.md,
//  frames [11] and [10]): advance the scheduler's clock, then pop and invoke
//  every timer node the advance made due.
//
//  Its own translation unit rather than an addition to scheduler.cpp:
//  DrainDueNodes calls InvokeSchedulerNodeCallback (frame [9]) for real, and
//  RebaseHeapNodeOffsets for real, and both live in scheduler.cpp - sharing
//  a TU would let /Ob2 inline either one and the shipped code calls both.
//============================================================================
#include "scheduler.h"

//  flt_6FAAE61C - the "is the leftover worth carrying" epsilon frame [11]
//  tests the post-horizon remainder against.  A plain hardware float, not a
//  CFloat: the shipped code `fld`s it directly.  Nothing else this repo has
//  reconstructed reads it, so it keeps its address for a name.
extern const float g_unk6FAAE61C;

//----------------------------------------------------------------------------
//  0x6F46E970 - frame [10].
//
//  The saved/restore of m_currentTime around the loop is the shipped
//  behaviour, not a defensive copy this reconstruction added: each pop
//  republishes m_currentTime as *that node's* fire time before invoking its
//  callback (which is what makes a callback's own "what time is it" read
//  answer the time it was scheduled for, not the frame's), and the caller's
//  value goes back at the end whether or not anything was popped.
//
//  The heap is 1-based - m_heapArray[1] is the root - which is why the live
//  count is compared against 1 rather than 0.
//----------------------------------------------------------------------------
void __thiscall SScheduler::DrainDueNodes()
{
    CFloat savedTime = m_currentTime;

    //  Spelled `!(fireTime <= savedTime)`, and both halves of that spelling
    //  are load-bearing.  The operand *order* decides which value the
    //  compiler loads first, so the more complex expression has to be on the
    //  left to get the shipped `fld [ecx+4]` before `fld [esp+..]`.  The
    //  *negation* decides the status-word mask: `!(a <= b)` is the one form
    //  that yields the shipped `test ah,1` / `jne`, where the positive
    //  `a > b` gets the NaN-safe `test ah,5` / `jnp` and `b < a` gets
    //  `test ah,41h` / `je`.  This is the same negated-comparison spelling
    //  the CItem save/load module already established
    //  (docs/msvc-vc8-idioms.md).
    while (m_liveCount > 1)
    {
        if (!(*(const float*)&m_heapArray[1]->m_fireTime.m_bits <=
              *(const float*)&savedTime.m_bits))
            break;

        SSchedulerHeapNode* node =
            ((SSchedulerHeapArrayHolder*)((char*)this + 4))->PopHeapNode();

        m_currentTime = node->m_fireTime;
        node->m_flags &= ~0x20000u;
        InvokeSchedulerNodeCallback(node);
    }

    m_currentTime = savedTime;
}

//----------------------------------------------------------------------------
//  0x6F46EAB0 - frame [11].
//
//  Two paths.  If the advanced clock still sits below the scheduler's
//  horizon, publish it and drain once - the ordinary frame.  If it has run
//  past the horizon, the clock cannot simply jump there, because every live
//  node's fire time is stored relative to the current epoch: drain up to the
//  horizon exactly, rebase every node's key by subtracting the horizon
//  (RebaseHeapNodeOffsets, which also bumps the epoch counter), then publish
//  the leftover and drain again.
//
//  `overshoot - g_CFloatZero` is not a no-op in this arithmetic: CFloat
//  flushes anything that would denormalise to zero, so subtracting zero is
//  how the shipped code normalises the remainder before measuring it.  The
//  measurement is on the absolute value (the sign bit is masked off with an
//  integer `and`, not with a CFloat operation), and a remainder smaller than
//  the epsilon is dropped to zero rather than carried.
//----------------------------------------------------------------------------
int __fastcall AdvanceSchedulerAndDrain(const CFloat& delta, SScheduler* scheduler)
{
    if (scheduler->m_scanFlags & 1)
        return 1;

    CFloat next = scheduler->m_currentTime + delta;

    //  Operand order and comparison sense both matter here, same as in
    //  DrainDueNodes above: `next` on the left is what puts its `fld` first,
    //  and the positive `<` is what gets the shipped `test ah,41h` / `jne`
    //  branching away to the overshoot path.
    if (*(const float*)&next.m_bits < *(const float*)&scheduler->m_horizon.m_bits)
    {
        scheduler->m_currentTime = next;
        scheduler->DrainDueNodes();
        return 1;
    }

    CFloat overshoot  = next - scheduler->m_horizon;
    CFloat normalised = overshoot - g_CFloatZero;

    normalised.m_bits &= 0x7FFFFFFF;

    if (*(const float*)&normalised.m_bits < g_unk6FAAE61C)
        overshoot = g_CFloatZero;

    scheduler->m_currentTime = scheduler->m_horizon;
    scheduler->DrainDueNodes();

    RebaseHeapNodeOffsets(scheduler);

    scheduler->m_currentTime = overshoot;
    scheduler->DrainDueNodes();
    return 1;
}
