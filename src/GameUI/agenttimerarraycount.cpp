//============================================================================
//  0x6F2FD240 - SAgentTimerArray::SetCount.
//
//  The growable-array shape this binary uses for arrays whose elements have
//  real constructors, and it is *not* Storm's TSGrowableArray::SetCount
//  (Containers/tsarray.inl): there is no chunk rounding, no shrink-side
//  destruction and no separate grow test.  It is four cases in one
//  sequence:
//
//    * the count is already right - do nothing at all, not even reallocate;
//    * the new count is zero - destroy the whole array through its own
//      destructor and clear all three words, so the array goes back to
//      looking freshly constructed;
//    * otherwise resize to exactly `count`, whether that is up or down.
//      Shrinking does not destroy the elements it drops here - SetAlloc
//      does that on the way past;
//    * and construct every element from the old count up to the new one.
//      A shrink makes that loop's bounds cross, so it runs zero times.
//
//  The null test in front of the element constructor is placement new's own,
//  not a hand-written one: the shipped code forms the element address, tests
//  it and skips the call.
//
//  The two stores of 0 and -1 into the frame's own unwind-state slot are the
//  __except_handler4 frame MSVC builds round a loop that constructs objects
//  - there is no __try in the source - and this repo's fixed /GS- /EHs-c-
//  emits neither them nor the prologue and funclet that go with them.  See
//  docs/msvc-vc8-idioms.md.
//
//  Own translation unit: the destructor, SetAlloc and the element
//  constructor are all real calls.
//============================================================================
#include "storm.h"
#include "agenttimer.h"

//----------------------------------------------------------------------------
//  The array, re-declared here against the same class name
//  GameUI/agenttimerarray.cpp uses - a local copy mangles identically,
//  because MSVC puts only the class *name* in a member's mangled name - and
//  with the element typed as the CAgentTimer it really is rather than as
//  that file's opaque 0x14 bytes, because this body constructs one.
//----------------------------------------------------------------------------
struct SAgentTimerArray
{
    //  0x6F2F7830 - GameUI/agenttimerarray.cpp.
    ~SAgentTimerArray();
    //  0x6F2FBE80 - resize the block to exactly `alloc` elements, destroying
    //  any that fall off the end and copy-constructing the survivors when
    //  Storm has to move it.  `retn 4`; still a redirect.
    void SetAlloc(unsigned int alloc);
    //  0x6F2FD240 - this file.
    void SetCount(unsigned int count);

    unsigned int m_alloc;   // +0x00
    unsigned int m_count;   // +0x04
    CAgentTimer* m_data;    // +0x08
};

void SAgentTimerArray::SetCount(unsigned int count)
{
    if (count == m_count)
        return;

    if (count == 0)
    {
        this->~SAgentTimerArray();
        m_alloc = 0;
        m_count = 0;
        m_data = 0;
        return;
    }

    SetAlloc(count);

    for (unsigned int i = m_count; i < count; i++)
        new (&m_data[i]) CAgentTimer();

    m_count = count;
}
