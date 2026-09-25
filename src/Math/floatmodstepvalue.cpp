//============================================================================
//  0x6F47C4D0 - SUnitTrackedRef::GetWrappedValue: the public accessor over
//  0x6F47C4B0, and its only caller.
//
//  Seven instructions and no arithmetic of its own: it forwards its own
//  hidden return buffer (the single stack argument - see floatmodstepget.cpp
//  for why a `retn 4` with no named parameter is exactly that) to
//  WrapCurrentValue and returns it.  The shipped code parks that buffer in
//  esi across the call and returns `mov eax, esi` rather than the callee's
//  own eax, which is what a by-value `return f();` through a hidden buffer
//  looks like.
//
//  Own translation unit: 0x6F47C4B0 is a real call.
//============================================================================
#include "unittrackedref.h"

CFloat SUnitTrackedRef::GetWrappedValue()
{
    return WrapCurrentValue();
}
