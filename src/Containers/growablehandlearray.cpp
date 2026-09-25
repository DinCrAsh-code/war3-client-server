//============================================================================
//  sub_6F62D1B0 (ComputeChunk<HANDLE>), sub_6F62D230 (SetAlloc<HANDLE>) and
//  sub_6F62E000 (Append) - see evtsched.h.  ComputeChunk/SetAlloc are the
//  same generic TSGrowableArray<T> members tsarray.cpp already instantiates
//  over a different T; kept in one TU the same way that file keeps its own
//  three template instantiations together (out-of-line member calls between
//  them survive at /O2 /Ob2 regardless of TU boundary once each is an
//  explicit instantiation, as tsarray.cpp already demonstrates).
//============================================================================
#include "evtsched.h"

//  TSGrowableArray<void*>::ComputeChunk/SetAlloc are already instantiated
//  in ptrarray.cpp - HANDLE is a typedef for void*, so this dump's
//  sub_6F62D1B0/sub_6F62D230 are the exact same compiled function reused at
//  a second pair of addresses (funcmap.py maps both IDA names to the one
//  C++ symbol), not a fresh instantiation.

//----------------------------------------------------------------------------
//  0x6F62E000 - grow the array's used count by exactly one, reusing
//  SetCount's own chunk-rounding arithmetic (m_chunk already known ?
//  use it : ComputeChunk(newCount); round newCount up to a multiple of that
//  chunk; SetAlloc if the rounded size does not already fit) and returning
//  a pointer to the freshly-available slot.  No per-element construct loop
//  because HANDLE is trivial - SetCount's own `new (&m_data[i]) T` is a
//  no-op for a plain pointer type, so it leaves no code here either.
//----------------------------------------------------------------------------
HANDLE* __fastcall HandleArrayAppend(TSGrowableArray<HANDLE>* array)
{
    unsigned int newCount = array->m_count + 1;

    if (newCount > array->m_alloc)
    {
        unsigned int chunk = array->m_chunk;
        if (chunk == 0)
            chunk = array->ComputeChunk(newCount);

        unsigned int alloc = newCount;
        unsigned int over = newCount % chunk;
        if (over != 0)
            alloc = newCount + (chunk - over);

        array->SetAlloc(alloc);
    }

    HANDLE* slot = &(*array)[array->m_count];
    array->m_count = newCount;
    return slot;
}
