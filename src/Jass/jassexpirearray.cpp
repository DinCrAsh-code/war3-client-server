//============================================================================
//  0x6F469F10 / 0x6F46A170 / 0x6F46AFA0 / 0x6F46C860 - four members of a
//  growable array of *pointers*, kept sorted by an expiration time
//  (`elem->m_when` at the pointee's own +0x04, a `float` compared through
//  the FPU) with a tiebreak (`elem->m_seq` at +0x14) - the classic shape of
//  a pending-callback/timer queue, though nothing in this batch's closure
//  ties it to a named class.  Each is `this` in ecx plus its own stack
//  arguments (thiscall's own shape - docs/msvc-vc8-idioms.md's
//  "`__thiscall` on a free function: rejected by this compiler" is why
//  these are real members of a placeholder class rather than free
//  functions), read off the four bodies' own shared displacements:
//
//    +0x0C  the pointer array's own data pointer
//    +0x14  a "reserve" high-water mark
//    +0x18  the allocated capacity
//    +0x1C  the count actually in use
//
//  This batch reconstructs the mechanics (grow-on-demand insertion,
//  removing a contiguous run, and a binary-heap sift used to keep the
//  array ordered) but not what the array is *for*, or the class it really
//  belongs to - `SExpirePtrArray` below is a placeholder name, not a
//  recovered one.  See the per-function notes and their BEHAVIOUR entries;
//  none of these four is claimed EXACT or even IDENTICAL, only "the same
//  operation, faithfully attempted".
//============================================================================
#include "storm.h"

extern "C" void* __cdecl memmove(void* dest, const void* src, unsigned int n);

struct SExpireEntry
{
    void*  m_reserved00;
    float  m_when;          // +0x04
    char   m_reserved08[0xC];
    int    m_seq;            // +0x14
};

class SExpirePtrArray
{
public:
    int   RemoveRange(unsigned int index, unsigned int count);   // 0x6F469F10
    int   Reserve(unsigned int count, unsigned int insertGap);   // 0x6F46A170
    void  SiftDown(void* entry);                                 // 0x6F46AFA0
    void* Insert();                                              // 0x6F46C860

    char   m_reservedNN[0xC];
    void** m_data;         // +0x0C
    unsigned int m_reserve; // +0x14 - "reserve" high-water mark
    unsigned int m_alloc;   // +0x18
    unsigned int m_count;   // +0x1C
};

//  0x6F469F10 - remove `count` pointers starting at `index`, closing the
//  gap with a memmove and shrinking the used count; `retn 8`.
int SExpirePtrArray::RemoveRange(unsigned int index, unsigned int count)
{
    unsigned int total = m_count;
    if (index >= total)
        return 0;

    unsigned int removeCount = count;
    if (index + count > total)
        removeCount = total - index;

    unsigned int keepCount = total - index - removeCount;
    if (keepCount != 0)
        memmove(&m_data[index], &m_data[index + removeCount], keepCount * sizeof(void*));

    m_count -= removeCount;
    return 1;
}

//  0x6F46A170 - reserve room for `count` more pointers past the current
//  count, growing the allocation (sub_6F4B39C0 - out of this batch's
//  scope) when the high-water mark at +0x14 would be exceeded, then
//  optionally shift the tail to open a `count`-pointer gap at the current
//  end (`insertGap` - the shipped body's own condition on its second
//  stack argument).  `retn 8`.
typedef int (__thiscall *GrowFn)(SExpirePtrArray*, unsigned int, int);

int SExpirePtrArray::Reserve(unsigned int count, unsigned int insertGap)
{
    if (count == 0)
        return 1;

    unsigned int used  = m_count;
    unsigned int need  = used + count;

    if (need > m_alloc)
    {
        unsigned int grow = m_reserve != 0 && m_reserve < need
                                 ? need - m_reserve : need - used;

        GrowFn growFn = (GrowFn)0;   // sub_6F4B39C0 - out of this batch's scope
        if (growFn && !growFn(this, grow * 4, 1))
            return 0;

        m_alloc += grow;
    }

    unsigned int base = used;

    if (insertGap)
    {
        // sub_6F4685B0 - out of this batch's scope: opens the gap in place.
    }

    m_count = base + count;
    return 1;
}

//  0x6F46AFA0 - sift-down: `entry` has just been placed at the end (index
//  m_count-1) and is walked toward the root of a max-heap ordered by
//  `m_when` (earliest-first: the comparison keeps the *later* child up,
//  the usual inversion for a min-heap built with `<=` compares), swapping
//  with the greater child until it settles or reaches the root.  `retn 4`.
void SExpirePtrArray::SiftDown(void* entry)
{
    unsigned int index = m_count - 2;

    if ((int)index <= 0)
    {
        m_data[0] = entry;
        return;
    }

    while (index > 1)
    {
        unsigned int parent = index / 2;
        SExpireEntry* parentEntry = (SExpireEntry*)m_data[parent];
        SExpireEntry* e = (SExpireEntry*)entry;

        int greater;
        if (parentEntry->m_when != e->m_when)
            greater = parentEntry->m_when > e->m_when;
        else
            greater = parentEntry->m_seq > e->m_seq;

        if (!greater)
            break;

        m_data[index] = m_data[parent];
        index = parent;
    }

    m_data[index] = entry;
}

//  0x6F46C860 - binary-search insertion point plus the same sift, over the
//  array's own [1 .. count-1) heap region (index 0 is a sentinel/root the
//  shipped body handles separately, `retn 0`).  Reconstructed only as far
//  as the mechanics go; not claimed exact.
void* SExpirePtrArray::Insert()
{
    unsigned int count = m_count;
    void* moved = m_data[count - 1];

    if (count >= 2)
    {
        unsigned int lo = 1;
        unsigned int hi = count - 1;

        while (lo < hi)
        {
            unsigned int mid = lo + (hi - lo) / 2;
            SExpireEntry* a = (SExpireEntry*)m_data[mid];
            SExpireEntry* b = (SExpireEntry*)m_data[hi];

            int greater;
            if (a->m_when != b->m_when)
                greater = a->m_when > b->m_when;
            else
                greater = a->m_seq > b->m_seq;

            if (!greater)
                lo = mid + 1;
        }

        m_data[hi] = m_data[lo];
        m_data[lo] = moved;
    }

    return moved;
}
