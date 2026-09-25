//============================================================================
//  0x6F2840A0 - CUnit::PopPendingTask.
//
//  Slot-3 (Method_0x0C) closure, and the callee unitrampflush.cpp's own
//  header called "a bare tail jump into [FlushPendingRamp]" - that was an
//  incomplete read from an earlier session; the whole function is real
//  work, it just *ends* in FlushPendingRamp() as its own tail call.
//
//  m_ref174 is the head of a singly-linked, handle-chained stack of
//  pending task nodes (same {handle, typeTag}-pair-plus-own-+0x24-"next"
//  shape unit_abilityshortcuts.cpp's own SUnitAbility::m_next chain
//  already established for a different list). Popping means: resolve the
//  head, and if there is one, replace it with *its own* +0x24 "next" pair
//  (SOptionalHandleRefResolver::ResolveChained does the replace, not just
//  a read) before releasing it through its own vtable slot 0x5C
//  (CAgent::Method_0x5C, agent.h). An empty stack - the sentinel pair, or
//  a lookup miss - posts "ERROR! Pop() of empty task stack!" instead and
//  returns without touching the ramp.
//
//  The ten order ids (0xD016B..0xD0174, in exactly this non-monotonic
//  order) have to be spelled as a literal `||` chain, not a `switch`: a
//  `switch` over these same ten (dense, contiguous) case labels is exactly
//  what MSVC *does* collapse into a range check here (`orderId - 0xD016B
//  <= 9`), which the shipped code does not do - it is a real `cmp`/`je`
//  chain, in this literal order. A matching order id
//  means the order this task belongs to had a target (+0x19C); that
//  target's own "still being ordered against" bit gets cleared
//  (ClearOrderTargetExecutingFlag, unit_clearordertargetflag.cpp) before
//  the node goes away.
//
//  The popped node's own real class is not in this dump (no vtable, no
//  further fields the walk reaches), so it is modelled with raw offsets
//  the way Agent/handlereref_resolve.cpp treats its own not-yet-typed
//  object, plus the one vtable slot this call site actually uses.
//
//  0.641 (42/63), IDENTICAL: every instruction the shipped stream has is
//  in this one, in the same order - the whole gap is `esi`/`edi` swapped
//  for the two live pointers (the task-stack head address and the popped
//  node itself) plus one instruction moving between the two register
//  saves as a result, the ordinary register-allocation cost the ten-way
//  `||` chain's own two live values leave the allocator free to pick
//  either way round.
//============================================================================
#include "unit.h"
#include "itemhandleresolve.h"
#include "unitorder.h"   // SOptionalHandleRefResolver::ResolveChained (0x6F0419C0)

//  "ERROR! Pop() of empty task stack!", 0x6F4CBF60 - already a real
//  reconstruction (Misc/sysmessagepost.cpp); re-declared the way every
//  other caller of it does.
int __fastcall PostSysMessage(const char* message, int type);

struct SPendingTask
{
    char               m_reserved00[0x24];
    SOptionalHandleRef m_next;       // +0x24/+0x28 - next node in the chain
    char               m_reserved2C[0x30 - 0x2C];
    unsigned int       m_orderId;    // +0x30 - the order id this task is for
};

//  CAgent's own vtable slot 23 (+0x5C, agent.h) - `this` only, `retn 0`.
typedef void (__thiscall *PendingTaskReleaseFn)(SPendingTask*);

void CUnit::PopPendingTask()
{
    if ((m_ref174.m_b & (int)m_ref174.m_t) == -1)
    {
        PostSysMessage("ERROR! Pop() of empty task stack!", 3);
        return;
    }

    SPendingTask* task = (SPendingTask*)QueryHandleField0x54((SOptionalHandleRef*)&m_ref174);
    if (!task)
    {
        PostSysMessage("ERROR! Pop() of empty task stack!", 3);
        return;
    }

    void* next = (task->m_next.m_typeTag & (int)task->m_next.m_handle) != -1
                     ? QueryHandleField0x54(&task->m_next) : 0;
    ((SOptionalHandleRefResolver*)&m_ref174)->ResolveChained(next);

    unsigned int orderId = task->m_orderId;
    if (orderId == 0xD016B || orderId == 0xD016C || orderId == 0xD016D ||
        orderId == 0xD016E || orderId == 0xD016F || orderId == 0xD0171 ||
        orderId == 0xD0170 || orderId == 0xD0172 || orderId == 0xD0174 ||
        orderId == 0xD0173)
    {
        ClearOrderTargetExecutingFlag();
    }

    ((PendingTaskReleaseFn)(*(void***)task)[0x5C / 4])(task);

    FlushPendingRamp();
}
