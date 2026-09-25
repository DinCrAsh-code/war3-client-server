//============================================================================
//  0x6F40B230 / 0x6F40B240 / 0x6F40B270 - three score fields that come out
//  of the tally list, with the walk written out rather than called.
//
//  Each is `mov ecx, [ecx+2D0h]` followed by a `jmp` into a body IDA gives
//  no heading of its own (0x6F3F64F0, 0x6F3F6540, 0x6F3F6660): those three
//  are tail *chunks* of these three functions, so their instructions land
//  in this function's own denominator and a `call` could never match them.
//  docs/msvc-vc8-idioms.md, "An IDA tail chunk can be recovered by inlining
//  - but only if its answer is used"; all three answers are used.
//
//  The two shapes in here are different on purpose and the diff says so:
//
//    * 0x6F40B230 / 0x6F40B240 select the contribution with an `if`/`else`
//      that has a **zero in the else arm** (`mov edx,[esi+1Ch]` / `jmp` /
//      `xor edx,edx`, then one shared `add edi,edx`).  Hoisting the zero
//      out and writing a guarded `total += ...` collapses that into a
//      single conditional add and loses two instructions per loop.
//    * 0x6F40B270 really is the guarded add, with no else arm at all.
//
//  All three read the flag word as a **byte**; the shipped code even parks
//  the mask in `al` across the loop.
//============================================================================
#include "playerscore.h"

//----------------------------------------------------------------------------
//  0x6F40B230 - the m_count1C total over the flag-8 entries.
//----------------------------------------------------------------------------
int SPlayerScore::TotalFlag8Count1C()
{
    CPlayerTallyList* list = m_pTallies;

    int total = 0;

    SPlayerTally* entry = list->Tail();
    while ((int)entry > 0)
    {
        {
            SPlayerTally* tally = entry;

            int add;
            if (tally->m_flags30 & 8)
                add = tally->m_count1C;
            else
                add = 0;
            total += add;

        }
        entry = list->Prev(entry);
    }

    return total;
}

//----------------------------------------------------------------------------
//  0x6F40B240 - the same over m_count2C.
//----------------------------------------------------------------------------
int SPlayerScore::TotalFlag8Count2C()
{
    CPlayerTallyList* list = m_pTallies;

    int total = 0;

    SPlayerTally* entry = list->Tail();
    while ((int)entry > 0)
    {
        {
            SPlayerTally* tally = entry;

            int add;
            if (tally->m_flags30 & 8)
                add = tally->m_count2C;
            else
                add = 0;
            total += add;

        }
        entry = list->Prev(entry);
    }

    return total;
}

//----------------------------------------------------------------------------
//  0x6F40B270 - the m_count2C total over the flag-4 entries, and this one
//  has no else arm.
//----------------------------------------------------------------------------
int SPlayerScore::TotalFlag4Count2C()
{
    CPlayerTallyList* list = m_pTallies;

    int total = 0;

    SPlayerTally* entry = list->Tail();
    while ((int)entry > 0)
    {
        {
            SPlayerTally* tally = entry;

            if (tally->m_flags30 & 4)
                total += tally->m_count2C;

        }
        entry = list->Prev(entry);
    }

    return total;
}
