//============================================================================
//  0x6F724F50 - clamp a requested chunk granularity to a power of two,
//  capped at 32.
//============================================================================

//----------------------------------------------------------------------------
//  Shape-only: nothing in this call tree names the owning class beyond "has
//  an unsigned int at +0xC this call reads and sometimes writes."  Same
//  idiom as Widget/positionradiussink.cpp's SPositionRadiusSink.
//----------------------------------------------------------------------------
struct SGranularityHolder
{
    unsigned int ClampGranularity(unsigned int requested);   // 0x6F724F50

    char         m_reserved00[0xC];
    unsigned int m_granularity;   // +0xC
};

//----------------------------------------------------------------------------
//  0x6F724F50 - `retn 4`, `this` in ecx.
//
//  At or above 32, the granularity latches at 32 and the field is stored -
//  the same "cap the chunk size" shape TSGrowableArray::ComputeChunk uses
//  (storm.h) for its own maxchunk branch.  Below 32 the request is floored
//  to the largest power of two not above it (repeatedly clearing the lowest
//  set bit until one remains - `x &= x-1` in a loop, same trick
//  ComputeChunk's own search below its cap does, run to convergence instead
//  of bounded by a `while` on the input) and clamped up to a minimum of 1,
//  but - unlike the >=32 branch - this result is *not* written back to
//  m_granularity, only returned.  Reproduced as written even though that
//  looks like a missing store: the shipped stream has no write on this
//  path, and matching it exactly is the point.
//----------------------------------------------------------------------------
unsigned int __thiscall SGranularityHolder::ClampGranularity(unsigned int requested)
{
    if (requested < 0x20)
    {
        unsigned int floored = requested;
        while (floored & (floored - 1))
            floored &= floored - 1;

        if (floored < 1)
            floored = 1;
        return floored;
    }

    m_granularity = 0x20;
    return 0x20;
}
