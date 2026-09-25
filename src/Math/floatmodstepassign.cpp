//============================================================================
//  0x6F47C4F0 - SUnitTrackedRef::AssignWrappedValue: publish a value that
//  has been wrapped into one step first, and hand `this` back so the call
//  site reads as an assignment.
//
//  Eight instructions around a call into 0x6F47C490, which is the same
//  wrap-then-publish pair one level down (CFloatModStep, floatmodstep.cpp,
//  then SUnitTrackedRef::SetValue).  The camera uses it for exactly the
//  properties that are angles - a wrap into one turn is what "mod step"
//  means for them - and SetValue for the ones that are not; see gameui.h.
//
//  Own translation unit: 0x6F47C490 is a real call.
//============================================================================
#include "unittrackedref.h"

SUnitTrackedRef* SUnitTrackedRef::AssignWrappedValue(const CFloat* value)
{
    SetWrappedValue(value);
    return this;
}
