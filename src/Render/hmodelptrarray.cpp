//============================================================================
//  0x6F5031E0 - resize a `void*`-element array (count @ +0x00, capacity
//  never re-read after the resize, data @ +0x08) to `newCount` elements,
//  trying an in-place SMemReAlloc first (Storm's 0x10 "resize here or fail"
//  flag - the same idiom Widget/animtrackarray.cpp's own SetAlloc already
//  documents) and falling back to a fresh SMemAlloc plus an element-by-
//  element copy of the surviving old pointers when that fails.  No caller
//  in this batch's own dump; the tag string ("aPauhmodel" in the dump,
//  an `.?AUHMODEL@@`-shaped RTTI name IDA could not fully decode) says the
//  element type is an HMODEL handle.
//
//  `retn 4` - one stack dword (the new count).  The new count is stored at
//  +0x00 *before* the reallocation is attempted (not after it succeeds),
//  which is the shipped order and is reproduced as such rather than
//  "cleaned up" to store it once at the end.
//============================================================================
#include "storm.h"

static const char s_hmodelArrayTag[] = ".?AUHMODEL@@";

struct SHModelPtrArray
{
    unsigned int m_count;      // +0x00
    unsigned int m_oldCount;   // +0x04 - read once, to clamp the copy length
    void**       m_data;       // +0x08

    void SetCount(unsigned int newCount);
};

void SHModelPtrArray::SetCount(unsigned int newCount)
{
    void** old = m_data;
    unsigned int size = newCount * 4;

    m_count = newCount;

    void** grown = (void**)SMemReAlloc(old, size, s_hmodelArrayTag, -2, 0x10);
    m_data = grown;

    if (grown == 0)
    {
        m_data = (void**)SMemAlloc(size, s_hmodelArrayTag, -2, 0);

        if (old != 0)
        {
            unsigned int keep = newCount;
            if (keep > m_oldCount)
                keep = m_oldCount;

            for (unsigned int i = 0; i < keep; i++)
            {
                void** slot = m_data + i;
                if (slot != 0)
                    *slot = old[i];
            }

            SMemFree(old, s_hmodelArrayTag, -2, 0);
        }
    }
}
