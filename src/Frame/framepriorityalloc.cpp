//============================================================================
//  0x6F60ADD0 / 0x6F60B020 - CFramePriorityArray::ComputeChunk and
//  ::SetAlloc, the two halves of the Storm growable array underneath one of
//  CSimpleTop's per-layer draw lists.
//
//  Neither calls the other, so they share a translation unit; SetCount and
//  EraseAt, which call both, are in their own.
//============================================================================
#include "frame.h"
#include "framethunks.h"

//  ".PAVFRAMEPRIORITY@@" - a *pointer* to FRAMEPRIORITY, which is what the
//  leading `.PAV` says.  File-local, not a funcmap.DATA name.
static const char kFramePriorityTypeName[] = ".PAVFRAMEPRIORITY@@";

//----------------------------------------------------------------------------
//  0x6F60ADD0 - the highest power of two not greater than `count`, capped at
//  64.  Only the capped answer is written back into m_chunk; below 64 the
//  field stays whatever it was, so the next grow recomputes.
//----------------------------------------------------------------------------
unsigned int CFramePriorityArray::ComputeChunk(unsigned int count)
{
    //  The capped answer is the *else*, not an early return: the shipped
    //  code branches forward onto it and falls through into the search
    //  (docs/msvc-vc8-idioms.md, "A clamp that branches *forward* to a
    //  shared `retn` is the `else`, not an early return").
    if (count < 64)
    {
        //  Clearing the lowest set bit until only one is left leaves the
        //  highest.
        unsigned int lower = (count - 1) & count;
        if (lower != 0)
        {
            do
            {
                count = lower;
                lower = (count - 1) & count;
            }
            while (lower != 0);
        }

        if (count >= 1)
            return count;

        return 1;
    }

    m_chunk = 64;
    return 64;
}

//----------------------------------------------------------------------------
//  0x6F60B020 - resize the storage.  Storm's flag 0x10 is "grow in place or
//  fail", so the fallback path allocates, copies and frees by hand.
//----------------------------------------------------------------------------
void CFramePriorityArray::SetAlloc(unsigned int alloc)
{
    FRAMEPRIORITY** old = m_data;

    m_alloc = alloc;
    m_data = (FRAMEPRIORITY**)SMemReAlloc(old, alloc * 4,
                                          kFramePriorityTypeName, -2, 0x10);

    if (m_data == 0)
    {
        m_data = (FRAMEPRIORITY**)SMemAlloc(alloc * 4, kFramePriorityTypeName,
                                            -2, 0);

        if (old != 0)
        {
            unsigned int keep = m_count;
            if (alloc < keep)
                keep = alloc;

            unsigned int i = 0;
            while (i < keep)
            {
                //  The shipped code forms the byte offset once and adds it
                //  to both bases, which is why the destination is computed
                //  as `m_data + offset` rather than as `m_data[i]`.
                unsigned int offset = i * 4;
                FRAMEPRIORITY** dst =
                    (FRAMEPRIORITY**)((char*)m_data + offset);
                if (dst != 0)
                    *dst = *(FRAMEPRIORITY**)((char*)old + offset);
                i += 1;
            }

            SMemFree(old, kFramePriorityTypeName, -2, 0);
        }
    }
}
