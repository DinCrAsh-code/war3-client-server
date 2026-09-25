//============================================================================
//  0x6F4D3960 - SAnimKeyArray::SetAlloc.
//
//  The *key* level of the same two-level structure animtrackarray.cpp
//  describes: SAnimTrack's own +0x08..+0x1C is one of these - a capacity, a
//  count, a pointer and two inline elements - and the same "two or fewer
//  live inline, more than two go to Storm" rule governs it, which is why
//  every capacity test compares against 2.
//
//  Same three shapes as its outer twin, one element type simpler because a
//  key is four bytes with nothing of its own to own:
//
//  * the grow path tries an in-place SMemReAlloc with Storm's 0x10 flag
//    first and returns immediately when that works, so the copy loop only
//    runs on a block that actually moved;
//  * shrinking to two elements or fewer parks the array back on its own
//    inline storage, and does nothing at all when it is already there;
//  * the copy loop tests the *computed destination address* rather than the
//    value, which is the shipped `jz` after the address arithmetic.
//
//  The Storm tag is asc_6FA4F788, the same descriptor string
//  animtrackarray.cpp passes at this level, and it is spelled as a static
//  literal here for the reason recorded there: this build has RTTI off, the
//  string is shared with other instantiations, and the operand
//  canonicalises to an anonymous symbol on both sides regardless.
//
//  Own translation unit: it is a real call from all three of its callers.
//============================================================================
#include "storm.h"

static const char s_animKeyTag[] =
    ".?AV?$TSFixedArray@UANIMKEY@@$0?CCCCCCCD@@@";

struct SAnimKeyArray
{
    unsigned int m_capacity;    // +0x00
    unsigned int m_count;       // +0x04
    int*         m_keys;        // +0x08
    int          m_inline[2];   // +0x0C

    void SetAlloc(unsigned int newCount);
};

void SAnimKeyArray::SetAlloc(unsigned int newCount)
{
    int* old = m_keys;

    if (newCount > 2)
    {
        if (m_capacity <= 2)
        {
            m_keys = (int*)SMemAlloc(newCount * 4, s_animKeyTag, -2, 0);
        }
        else
        {
            //  Storm's 0x10 flag: resize where it is or fail, never move.
            int* grown = (int*)SMemReAlloc(old, newCount * 4, s_animKeyTag,
                                           -2, 0x10);
            m_keys = grown;
            if (grown != 0)
            {
                m_capacity = newCount;
                return;
            }

            m_keys = (int*)SMemAlloc(newCount * 4, s_animKeyTag, -2, 0);
        }
    }
    else
    {
        if (old == m_inline)
        {
            m_capacity = newCount;
            return;
        }
        m_keys = m_inline;
    }

    m_capacity = newCount;
    if (old == 0)
        return;

    unsigned int keep = newCount;
    if (keep >= m_count)
        keep = m_count;

    //  A plain `for`, not `if (keep > 0) { for ... }`: the shipped stream
    //  tests the count exactly once, and the guarded form makes MSVC emit
    //  the test twice.
    for (unsigned int i = 0; i < keep; i++)
    {
        int* slot = m_keys + i;
        if (slot != 0)
            *slot = old[i];
    }

    if (old != m_inline)
        SMemFree(old, s_animKeyTag, -2, 0);
}
