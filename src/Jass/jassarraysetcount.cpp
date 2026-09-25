//============================================================================
//  0x6F45A100 - JassArray::SetCount.  The generic TSGrowableArray<T>::
//  SetCount shape (Containers/tsarray.inl) with two differences the shipped
//  stream itself demands: the requested count is clamped to 0x2000 first
//  (JASS's own array-size cap), and growing zero-fills the new slots with
//  one `memset` rather than a per-element placement-new loop - the same
//  substitution the generic template's own callers get for a trivial T,
//  except here it is written directly rather than relying on the compiler
//  to fold an empty loop, which is what the disassembly's own single
//  `memset` call (not a loop) confirms.
//============================================================================
#include "storm.h"
#include "jassarray.h"

extern "C" void* __cdecl memset(void* dst, int val, unsigned int size);

void JassArray::SetCount(unsigned int count)
{
    if (count > 0x2000)
        count = 0x2000;

    unsigned int oldCount = m_elementCount;

    //  Grow-only, and the growth target is max(count,oldCount) rather than
    //  count itself: a smaller `count` still writes m_elementCount back,
    //  just with the value it already held, which is why nothing here
    //  ever frees or destroys a live element.
    unsigned int newCount = (count > oldCount) ? count : oldCount;

    if (newCount > oldCount)
    {
        TSGrowableArray<void*>* elements = (TSGrowableArray<void*>*)&m_elementAlloc;
        if (newCount > m_elementAlloc)
        {
            unsigned int chunk = m_elementChunk;
            if (chunk == 0)
                chunk = elements->ComputeChunk(newCount);

            unsigned int alloc = newCount;
            unsigned int over = newCount % chunk;
            if (over != 0)
                alloc = newCount + (chunk - over);

            elements->SetAlloc(alloc);
        }
    }

    m_elementCount = newCount;

    if (newCount > oldCount)
    {
        unsigned int bytes = (newCount - oldCount) * 4;
        memset(m_elementData + oldCount, 0, bytes);
    }
}
