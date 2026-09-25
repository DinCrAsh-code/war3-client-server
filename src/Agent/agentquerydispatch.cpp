//============================================================================
//  0x6F47FAF0 / 0x6F47FB50 - pick which of the three type filters an agent
//  enumeration runs with.
//
//  `type` of 0 or -1 is "any type", and then neither the exact test nor the
//  derived-from test is compiled into the walk at all; otherwise `derived`
//  chooses between them.  Three separate leaves rather than one with two
//  branches, which is why there are six enumerator bodies and not one.
//
//  The two centre coordinates stay in ecx and edx the whole way down from
//  EnumerateUnitsInRect (agentqueryrect.cpp) and are pushed here as the
//  leaf's first two stack words; the radius becomes the leaf's `this`.
//
//  Own translation unit: both call the six leaves for real.
//============================================================================
#include "agentquery.h"

//----------------------------------------------------------------------------
//  0x6F47FAF0 - the candidate's centre must be inside the circle.
//----------------------------------------------------------------------------
void __fastcall EnumerateAgentsInCircle(const int* x, const int* y,
                                        const int* radius, unsigned int mask,
                                        unsigned int type, int derived,
                                        UnitQueryVisitFn fn, void* ctx)
{
    const SQueryRadius* r = (const SQueryRadius*)radius;

    if (type != 0 && type != 0xFFFFFFFF)
    {
        if (derived != 0)
            r->EnumerateInCircleDerivedFrom(x, y, mask, type, fn, ctx);
        else
            r->EnumerateInCircleOfType(x, y, mask, type, fn, ctx);
    }
    else
    {
        r->EnumerateInCircle(x, y, mask, fn, ctx);
    }
}

//----------------------------------------------------------------------------
//  0x6F47FB50 - the candidate's own collision circle need only touch it.
//----------------------------------------------------------------------------
void __fastcall EnumerateAgentsTouchingCircle(const int* x, const int* y,
                                              const int* radius,
                                              unsigned int mask,
                                              unsigned int type, int derived,
                                              UnitQueryVisitFn fn, void* ctx)
{
    const SQueryRadius* r = (const SQueryRadius*)radius;

    if (type != 0 && type != 0xFFFFFFFF)
    {
        if (derived != 0)
            r->EnumerateTouchingCircleDerivedFrom(x, y, mask, type, fn, ctx);
        else
            r->EnumerateTouchingCircleOfType(x, y, mask, type, fn, ctx);
    }
    else
    {
        r->EnumerateTouchingCircle(x, y, mask, fn, ctx);
    }
}
