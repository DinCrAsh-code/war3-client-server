//============================================================================
//  NetHandlePairArray - the exact-size (no chunking) growable array of
//  8-byte (int,int) pairs CNetCommandUnitSelectionModify/
//  CNetCommandUnitDefineControlGroup keep at +0x1C (see netcommand_dispatch.cpp
//  for the two owning classes and their Attach hooks).  Not an instantiation
//  of the generic TSGrowableArray<T> template (tsarray.inl) - that template
//  carries a fourth m_chunk field and grows in ComputeChunk() batches; this
//  type has only alloc/count/data (three dwords, confirmed by the owning
//  classes' own field layout) and SetCount always reallocates to exactly the
//  requested count, with no headroom.
//============================================================================
#ifndef NETHANDLEPAIRARRAY_H
#define NETHANDLEPAIRARRAY_H

#include "storm.h"

//  The RTTI tag the shipped code hands Storm for every alloc/realloc/free
//  of this array's storage (IDA's aAunettag, ".?AUNETTAG@@").
#define NETHANDLEPAIRARRAY_TAG ".?AUNETTAG@@"

//  A non-trivial default constructor (fields born -1/-1, "no handle") is
//  what makes SetCount's per-new-element store carry MSVC's placement-new
//  null guard in the shipped disassembly.
struct NetHandlePair
{
    NetHandlePair() : m_lo(-1), m_hi(-1) {}
    int m_lo;
    int m_hi;
};

class NetHandlePairArray
{
public:
    //  0x6F2C9700 - reallocate storage to hold exactly `alloc` elements,
    //  moving any surviving ones.  No per-element destroy/construct: T's
    //  copy is a raw 8-byte move, matching the disassembly's own straight
    //  dword-pair copy loop.
    void SetAlloc(unsigned int alloc);

    //  0x6F2CEC00 - change the element count.  Growing (or shrinking to a
    //  still-nonzero count) reallocates to exactly `count` and default-
    //  constructs any newly-added tail elements; dropping to zero frees the
    //  storage outright and resets every field, rather than merely
    //  destroying the surviving elements the way the generic template's
    //  SetCount would.
    void SetCount(unsigned int count);

    unsigned int   m_alloc;   // +0x0
    unsigned int   m_count;   // +0x4
    NetHandlePair* m_data;    // +0x8
};

#endif
