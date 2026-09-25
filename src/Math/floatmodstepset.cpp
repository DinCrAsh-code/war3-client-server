//============================================================================
//  0x6F47C490 - SUnitTrackedRef::SetWrappedValue: wrap `value` into one step
//  and publish that.
//
//  Ten instructions and two calls: CFloatModStep builds the wrapped value
//  and SetValue publishes it.  The wrapped value never gets a stack slot of
//  its own - the shipped `lea ecx, [esp+4+arg_0]` hands CFloatModStep the
//  *incoming argument's* own slot as the hidden return buffer, which is
//  what a temporary bound to a const reference for the length of the full
//  expression gets you and what a named local does not.  AddressOf() below
//  is the same spelling floatmodstep.cpp already uses for that, and for the
//  same reason.
//
//  Was a redirect in GameUI/gameui_thunks.cpp until the camera family
//  needed the wrapping setters to be real bodies.
//
//  Own translation unit: CFloatModStep and SetValue are both real calls.
//============================================================================
#include "unittrackedref.h"

//  0x6F47C420 - floatmodstep.cpp.  Re-declared, not redefined.
CFloat __fastcall CFloatModStep(const CFloat& value);

//  Binds a temporary to a const reference so its address is the address the
//  callee's own return buffer already occupies - see the header comment.
static const CFloat* AddressOf(const CFloat& v) { return &v; }

void SUnitTrackedRef::SetWrappedValue(const CFloat* value)
{
    SetValue(AddressOf(CFloatModStep(*value)));
}
