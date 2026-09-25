//============================================================================
//  See scheduler.h.  CMissileThunderBolt impact pipeline, frame [9] (node
//  invoke), plus the rearm and epoch-rebase helpers it and the drain call
//  (docs/targets/cmissilethunderbolt_impact_pipeline.md).
//============================================================================
#include "scheduler.h"

//----------------------------------------------------------------------------
//  0x6F46C860 - pop-heap.  Not reconstructed (124 instructions of sift-down
//  this session did not get to); naked jmp thunk to the shipped address, the
//  same shape PushHeapNode below already uses, so build_mix.py's full-DLL
//  link resolves the call DrainDueNodes (scheduler_drain.cpp) makes.
//----------------------------------------------------------------------------
__declspec(naked) SSchedulerHeapNode* __thiscall SSchedulerHeapArrayHolder::PopHeapNode()
{
    __asm { mov eax, 0x6F46C860 }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F46AFA0 - push-heap insert (scheduler.h's own declaration).  Not yet
//  reconstructed.  Naked jmp thunk to the real shipped address (same
//  pattern as src/unreconstructed_thunks.cpp's MEMBER_THUNK /
//  misc_adjuster_thunks.cpp) so build_mix.py's full-DLL link resolves it -
//  verify.py never links, so a declared-only extern compiles clean here and
//  only fails there.
//----------------------------------------------------------------------------
__declspec(naked) void __thiscall SSchedulerHeapArrayHolder::PushHeapNode(SSchedulerHeapNode*)
{
    __asm { mov eax, 0x6F46AFA0 }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F46D5B0 - recompute a node's fire time as scheduler->currentTime +
//  node->period, twiddle its flags (clear the "just popped" bit, set the
//  "armed" bit) and re-insert it into the heap.  __stdcall: this is edi
//  (the scheduler), the node comes off the stack.
//----------------------------------------------------------------------------
void __thiscall SScheduler::RearmSchedulerNode(SSchedulerHeapNode* node)
{
    node->m_fireTime = m_currentTime + node->m_period;
    node->m_flags = (node->m_flags & ~0x10000u) | 0x20000u;
    ((SSchedulerHeapArrayHolder*)((char*)this + 4))->PushHeapNode(node);
}

//----------------------------------------------------------------------------
//  0x6F46E4D0 - invoke a popped node's callback (CAgentTickRelay::Tick,
//  vtable slot 0x48/4=18), then either re-arm it (periodic) or release it
//  back to the scheduler's free list (state byte bit 0 set - a one-shot
//  node).  Single pointer argument, so __fastcall (ecx-only) reproduces
//  the same calling convention as __thiscall with no other arguments.
//----------------------------------------------------------------------------
typedef void (__thiscall *SchedulerNodeTickFn)(void* callback, SSchedulerHeapNode* node);

void __fastcall InvokeSchedulerNodeCallback(SSchedulerHeapNode* node)
{
    if (!(node->m_flags & 0x10000))
    {
        void* callback = node->m_callback;
        void* vtable = *(void**)callback;
        SchedulerNodeTickFn tick = *(SchedulerNodeTickFn*)((char*)vtable + 0x48);
        tick(callback, node);

        unsigned int flags = node->m_flags;
        if ((flags & 1) && !(flags & 0x10000))
        {
            SScheduler* scheduler = (SScheduler*)node->m_scheduler;
            scheduler->RearmSchedulerNode(node);
        }
        return;
    }

    SScheduler* scheduler = (SScheduler*)node->m_scheduler;
    void* freeListNext = scheduler->m_freeListHead;
    *(void**)((char*)node - 4) = freeListNext;
    scheduler->m_reserved3C -= 1;
    scheduler->m_freeListHead = (char*)node - 4;
}

//----------------------------------------------------------------------------
//  0x6F469CD0 - epoch rebase: subtract the scheduler's own horizon from
//  every live node's fire time (indices [1, m_liveCount) - index 0 is the
//  heap root and not touched here) and bump the epoch counter by exactly
//  one either way, whether or not the loop actually ran.
//----------------------------------------------------------------------------
void __fastcall RebaseHeapNodeOffsets(SScheduler* scheduler)
{
    unsigned int i = 1;

    if (scheduler->m_liveCount > 1)
    {
        do
        {
            SSchedulerHeapNode* node = scheduler->m_heapArray[i];
            node->m_fireTime = node->m_fireTime - scheduler->m_horizon;
            i += 1;
        } while (i < scheduler->m_liveCount);

        scheduler->m_epoch += 1;
    }
    else
    {
        scheduler->m_epoch += i;
    }
}
