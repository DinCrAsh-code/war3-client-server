//============================================================================
//  0x6F612830 - SGridRow::SetAlloc and 0x6F612D70 - SGridRowArray::SetCount.
//  Both are real reconstructions now (naked redirects in
//  Frame/framethunks.cpp before this batch); both are ordinary
//  TSGrowableArray-shaped growth code (Containers/tsarray.inl documents the
//  same SMemReAlloc-then-fall-back-to-alloc/copy/free shape and the same
//  grow/shrink SetCount split) written out longhand rather than through
//  that template, because SGridRow's own element type is `CSimpleFrame*`
//  (a raw pointer with nothing for a destructor to do) while
//  SGridRowArray's element type is `SGridRow` itself (which *does* have
//  something to free per element) - two different instantiations, and nothing
//  else in this closure needs either of them as a real TSGrowableArray<T>.
//============================================================================
#include "frame.h"
#include "storm.h"

//  0x6F612830 - grow/shrink this row's own cell array.  Ask Storm to resize
//  in place first; on failure, allocate fresh, move the surviving pointers
//  over one at a time (bytewise would be fine too - they are plain
//  pointers - but the shipped stream copies index by index) and free the
//  old block.  0.706 (36/52): the copy loop's own induction-variable shape
//  differs (the shipped body re-derives the new block's address from
//  `this` every iteration and folds a stray "is the computed address
//  itself zero" check into the loop from that; this build hoists the base
//  pointer once and loops incrementing/comparing instead of the shipped
//  decrement-style walk) - same values copied, same iteration count, no
//  branch or field differs; pending angr IDENTICAL verification.
void SGridRow::SetAlloc(unsigned int alloc)
{
    CSimpleFrame** olddata = m_cells;

    m_alloc = alloc;
    m_cells = (CSimpleFrame**)SMemReAlloc(olddata, alloc * sizeof(CSimpleFrame*),
                                         ".PAVCSimpleFrame@@", -2, 0x10);
    if (m_cells == 0)
    {
        //  Cached once rather than read back through `m_cells` inside the
        //  loop below: the shipped stream computes the new block's own
        //  base address a single time, immediately after this store, and
        //  the copy loop just walks it - not re-deriving it from `this`
        //  every iteration.
        CSimpleFrame** newdata = (CSimpleFrame**)SMemAlloc(
            alloc * sizeof(CSimpleFrame*), ".PAVCSimpleFrame@@", -2, 0);
        m_cells = newdata;
        if (olddata != 0)
        {
            unsigned int moved = alloc < m_count ? alloc : m_count;
            for (unsigned int i = 0; i < moved; i++)
                newdata[i] = olddata[i];
            SMemFree(olddata, ".PAVCSimpleFrame@@", -2, 0);
        }
    }
}

//  0x6F612300 / 0x6F612B20 - the row array's own ComputeChunk/SetAlloc
//  (the "grow the array of SGridRow" pair SetCount below calls) - another
//  module's, not one of this batch's 35 addresses.  Declared and thunked
//  here purely so SetCount has real targets to call, the same pattern this
//  whole batch's other files use for an out-of-scope leaf.
class SGridRowArrayGrow
{
public:
    unsigned int ComputeChunk(unsigned int count);   // 0x6F612300, `retn 4`
    void SetAlloc(unsigned int alloc);               // 0x6F612B20, `retn 4`
};

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

ADDR_THUNK(unsigned int SGridRowArrayGrow::ComputeChunk(unsigned int), 0x6F612300)
ADDR_THUNK(void SGridRowArrayGrow::SetAlloc(unsigned int), 0x6F612B20)

//  0x6F612D70 - set the row count: grow the backing array first if needed
//  (rounding up to the current chunk granularity, computing one if none is
//  latched yet), default-construct (zero) every row the growth adds, or
//  free each dropped row's own cell array on the way down.  0.883 (68/78):
//  register-allocation churn around the growth-arithmetic block (the
//  shipped body keeps `count` itself live in one register throughout where
//  this build reloads it once) and the final `m_count = count` store
//  landing before vs. after this function's own epilogue pops - same
//  comparisons, same arithmetic, same store; pending angr IDENTICAL
//  verification.
void SGridRowArray::SetCount(unsigned int count)
{
    if (count > m_count)
    {
        if (count > m_alloc)
        {
            unsigned int chunk = m_chunk;
            if (chunk == 0)
                chunk = ((SGridRowArrayGrow*)this)->ComputeChunk(count);

            unsigned int alloc = count;
            unsigned int over = count % chunk;
            if (over != 0)
                alloc = count + (chunk - over);

            ((SGridRowArrayGrow*)this)->SetAlloc(alloc);
        }

        //  One pointer computed per row, not re-read from `m_rows` for each
        //  of the four fields: the shipped stream takes `&m_rows[i]` once
        //  and stores all four zeroes through it.
        for (unsigned int i = m_count; i < count; i++)
        {
            unsigned int* row = (unsigned int*)&m_rows[i];
            row[0] = 0;
            row[1] = 0;
            row[2] = 0;
            row[3] = 0;
        }
    }
    else if (count < m_count)
    {
        for (unsigned int i = count; i < m_count; i++)
        {
            if (m_rows[i].m_cells != 0)
                SMemFree(m_rows[i].m_cells, ".PAVCSimpleFrame@@", -2, 0);
        }
    }

    m_count = count;
}
