//============================================================================
//  0x6F467570 - EnumerateUnitsInRect.
//
//  BuildAndRegisterEventBinding (agenteventbinding.cpp) reaches this with
//  three CFloats it collected from one variadic tag: a radius and a centre.
//  Despite the name the tag's own shape gave it, nothing rectangular happens
//  here - the rectangle is an internal step, the cell box that circumscribes
//  the circle (agentqueryenumerate.cpp).  The name is the one the header and
//  every existing call site already carry.
//
//  All this does is choose the distance rule: `altForm` set means a
//  candidate counts when its own collision circle touches the query circle,
//  clear means its centre has to be inside.  Both dispatchers take the same
//  six stack words, which is why the shipped code pushes them once, above
//  the branch, and only duplicates the call and the return.
//
//  Own translation unit: it calls both dispatchers for real.
//============================================================================
#include "agentquery.h"

void __fastcall EnumerateUnitsInRect(const int* lo, const int* hi,
                                     const int* origin, int altForm,
                                     unsigned int mask, int a, int b,
                                     UnitQueryVisitFn fn, void* ctx)
{
    if (altForm != 0)
        EnumerateAgentsTouchingCircle(lo, hi, origin, mask,
                                      (unsigned int)a, b, fn, ctx);
    else
        EnumerateAgentsInCircle(lo, hi, origin, mask,
                                (unsigned int)a, b, fn, ctx);
}
