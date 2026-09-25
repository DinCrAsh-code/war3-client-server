//============================================================================
//  0x6F2FBE80 - SAgentTimerArray::SetAlloc: change the block's size.
//
//  Three things happen, in this order, and the order is what the body is:
//
//  * every element that falls off the end - index >= the new allocation - is
//    destroyed *first*, through the vtable rather than by name, so that the
//    block Storm is about to shrink no longer owns anything;
//  * Storm is asked to resize in place (flag 0x10, "move it and I would
//    rather you failed"), and when that works the function is done: the
//    surviving elements never moved, so nothing has to be copied;
//  * only when that fails is a fresh block allocated and the survivors
//    copy-constructed into it one at a time, each source destroyed
//    immediately after its copy, and the old block freed.
//
//  The survivor count is `min(newAlloc, m_count)`, which is what makes a
//  grow copy every live element and a shrink copy only the ones that are
//  still in range - the ones past it were already destroyed above.
//
//  The copy is CAgentTimer's *implicit* copy constructor and every step of
//  it is in the shipped stream: TRefCnt's own copy stamps ??_7TRefCnt@@6B@
//  and carries the refcount word across, the derived stamp
//  ??_7CAgentTimer@@6B@ goes over it, CAgentPtr's copy constructor takes a
//  reference on the target if there is one, and the queue pointer and the
//  flags follow. Writing it out by hand would be the same instructions and
//  a worse source.
//
//  The two destructions go through vtable slot 1 with a zero flag - MSVC's
//  scalar deleting destructor, told not to free the storage, because the
//  storage belongs to the array.  Spelled as a hand-written dispatch and not
//  as `~CAgentTimer()` because the shipped code dispatches: a direct call is
//  what the destructor's static type would give.  **The receiver is an
//  element pointer and not `this`, so tools/vtable_dispatch_audit.py cannot
//  judge either site, and CAgentTimer has no committed vtable to check them
//  against.**  What fixes the argument count is the shipped call sites
//  themselves (0x6F2FBEA5 and 0x6F2FBF4E): both push exactly one dword and
//  set only ecx.
//
//  Own translation unit: the element destructor, both Storm calls and the
//  copy's own CAgentPtr constructor are real calls.
//============================================================================
#include "storm.h"
#include "agenttimer.h"

//  The RTTI descriptor name the shipped allocator calls are tagged with,
//  reproduced as a literal because this build has RTTI off - the same tag
//  and the same reasoning as GameUI/agenttimerarray.cpp's.
static const char kAgentTimerTag[] = "AVCAgentTimer";

//  Slot 1 (+0x04) of a CAgentTimer - the scalar deleting destructor, told
//  with a zero flag not to free the storage.
static void DestroyElement(CAgentTimer* element)
{
    typedef void (__thiscall *DestroyFn)(void*, int);
    ((DestroyFn)(*(void***)element)[0x04 / 4])(element, 0);
}

struct SAgentTimerArray
{
    void SetAlloc(unsigned int alloc);

    unsigned int m_alloc;   // +0x00
    unsigned int m_count;   // +0x04
    CAgentTimer* m_data;    // +0x08
};

void SAgentTimerArray::SetAlloc(unsigned int alloc)
{
    CAgentTimer* old = m_data;

    if (alloc < m_count)
    {
        for (unsigned int i = alloc; i < m_count; i++)
            DestroyElement(&old[i]);
    }

    unsigned int bytes = alloc * sizeof(CAgentTimer);

    m_alloc = alloc;
    m_data = (CAgentTimer*)SMemReAlloc(old, bytes, kAgentTimerTag, -2, 0x10);
    if (m_data != 0)
        return;

    m_data = (CAgentTimer*)SMemAlloc(bytes, kAgentTimerTag, -2, 0);
    if (old == 0)
        return;

    unsigned int survivors = m_count;
    if (alloc < survivors)
        survivors = alloc;

    if (survivors != 0)
    {
        for (unsigned int i = 0; i < survivors; i++)
        {
            //  The null test is placement new's own: the shipped code forms
            //  the destination address, tests it and skips only the copy -
            //  the source is destroyed either way.
            CAgentTimer* slot = &m_data[i];
            if (slot != 0)
                new (slot) CAgentTimer(old[i]);

            DestroyElement(&old[i]);
        }
    }

    SMemFree(old, kAgentTimerTag, -2, 0);
}
