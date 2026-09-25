//============================================================================
//  0x6F47C4B0 - SUnitTrackedRef::WrapCurrentValue: read the value this
//  reference publishes and hand back the same value wrapped into one step.
//
//  The answer is returned **by value**, which is what the single stack
//  argument and the `retn 4` are: MSVC's hidden return buffer for a class
//  return arrives as the first stack slot of a __thiscall, and the shipped
//  body never stores anything into it itself - it hands it straight to
//  CFloatModStep as *that* call's own return buffer (`mov ecx, esi` before
//  the call) and returns it in eax.  Reading it instead as an out-pointer
//  plus a copy costs the copy, and nothing else in the stream would move.
//
//  The intermediate is a genuinely uninitialised local: the shipped frame is
//  one `push ecx` and GetValue writes it before anything reads it, so the
//  zeroing default constructor would be a dead store here (CFloat.h's
//  kCFloatNoInit tag).
//
//  Own translation unit: GetValue and CFloatModStep are both real calls.
//============================================================================
#include "unittrackedref.h"

//  0x6F47C420 - floatmodstep.cpp.  Re-declared, not redefined.
CFloat __fastcall CFloatModStep(const CFloat& value);

CFloat SUnitTrackedRef::WrapCurrentValue()
{
    CFloat current(kCFloatNoInit);

    //  GetValue hands its own out-pointer back in eax, and the shipped code
    //  uses that returned pointer as CFloatModStep's operand rather than
    //  taking the local's address a second time - which is why this is
    //  written as one expression and not as two statements.
    return CFloatModStep(*GetValue(&current));
}
