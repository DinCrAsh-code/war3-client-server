//============================================================================
//  The scheduler heap driving the CMissileThunderBolt impact pipeline
//  (docs/targets/cmissilethunderbolt_impact_pipeline.md) - a float-keyed
//  min-heap of timer nodes, drained once per frame.  Field offsets are the
//  ones the pipeline notes already document (S = *(*(0x6FAB7368)+0x40)).
//
//  Only the fields the three functions below actually touch are declared;
//  the drain/pop pair above them in the pipeline (sub_6F46EAB0/
//  sub_6F46E970) and the generic heap sift-down/insert primitives they call
//  (sub_6F46C860/sub_6F469F10/sub_6F46AFA0) are shared scheduler
//  infrastructure, not spell-specific, and are left for a follow-up session
//  rather than guessed at here.
//============================================================================
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "CFloat.h"

struct SSchedulerHeapNode
{
    void*        m_link;        // +0x00 - 0 while live
    CFloat       m_fireTime;    // +0x04 - heap key
    CFloat       m_period;      // +0x08
    void*        m_scheduler;   // +0x0C - back-pointer to the owning scheduler
    //  +0x10 - bit 0x20000 cleared on pop, set on rearm; bit 16 (the byte at
    //  +0x12, read/tested separately in the target listing rather than as
    //  part of this dword - same field, byte-granular access) selects
    //  release-to-freelist over invoke.
    unsigned int m_flags;       // +0x10
    unsigned int m_counterId;   // +0x14
    void*        m_callback;    // +0x18 - CAgentTickRelay* (vtable slot 0x48 = Tick)
    void*        m_param;       // +0x1C
    void*        m_siblingLink; // +0x20
};

struct SScheduler
{
    unsigned char m_reserved00[0x10];
    SSchedulerHeapNode** m_heapArray;   // +0x10
    unsigned char m_reserved14[0xC];
    unsigned int m_liveCount;           // +0x20
    unsigned char m_reserved24[0x14];
    void*        m_freeListHead;        // +0x38 - old head read, then overwritten with the released node
    unsigned int m_reserved3C;          // +0x3C - decremented on release
    CFloat       m_currentTime;         // +0x40
    unsigned int m_epoch;               // +0x44 - bumped by RebaseHeapNodeOffsets
    CFloat       m_horizon;             // +0x48
    unsigned int m_scanFlags;           // +0x4C - bit 0 gates the drain

    //  0x6F46D5B0 - recompute a node's fire time as currentTime +
    //  node->period, twiddle its flags (clear the "just popped" bit, set
    //  the "armed" bit) and re-insert it into the heap.
    void __thiscall RearmSchedulerNode(SSchedulerHeapNode* node);

    //  0x6F46E970 - pipeline frame [10].  Pop and invoke every node whose
    //  fire time has already passed, republishing m_currentTime as each
    //  popped node's own fire time while its callback runs and restoring
    //  the caller's value afterwards.
    void __thiscall DrainDueNodes();
};

//  0x6F46AFA0 - push-heap insert.  Not yet reconstructed; declared only so
//  RearmSchedulerNode below compiles and matches its own argument setup.
//  The real call site's `this` is a pointer into the scheduler (S+4), not a
//  named type - modelled as a member of an anonymous holder rather than a
//  free function so the calling convention (this=ecx, node on the stack)
//  is exact.
struct SSchedulerHeapArrayHolder
{
    void __thiscall PushHeapNode(SSchedulerHeapNode* node);

    //  0x6F46C860 - pop-heap: detach the root (m_heapArray[1] - this heap is
    //  1-based, which is why every live-count test in this file compares
    //  against 1 rather than 0) and sift the last element down into its
    //  place.  Returns the detached node.  Not reconstructed - see
    //  scheduler.cpp.
    SSchedulerHeapNode* __thiscall PopHeapNode();
};

//  0x6F469CD0 / 0x6F46E4D0 - scheduler.cpp.
void __fastcall RebaseHeapNodeOffsets(SScheduler* scheduler);
void __fastcall InvokeSchedulerNodeCallback(SSchedulerHeapNode* node);

//  0x6F46EAB0 - pipeline frame [11], the outermost frame of the impact
//  pipeline: advance the scheduler's clock by `delta` and drain whatever
//  that makes due.  __fastcall with the delta in ecx and the scheduler in
//  edx - not a member of either, and not a __thiscall on the scheduler.
int __fastcall AdvanceSchedulerAndDrain(const CFloat& delta, SScheduler* scheduler);

#endif
