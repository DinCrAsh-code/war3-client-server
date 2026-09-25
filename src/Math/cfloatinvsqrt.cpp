//============================================================================
//  0x6F6EF800 - CFloatInvSqrt: the reciprocal of a square root, as one
//  encoded-float operation.
//
//  Its whole body is `CFloatReciprocal(CFloatSqrt(value))`.  The shipped
//  stream is the plain nested-call shape that produces: the caller's return
//  buffer arrives in ecx and is parked in esi across both calls, the inner
//  sqrt is given one dword of stack for its own result (`push ecx` is the
//  frame, not an argument), and the operand is never reloaded because it is
//  already in edx where __fastcall put it and CFloatSqrt takes it there too.
//
//  Its own translation unit and not folded into pathsqrt.cpp: CFloatSqrt is
//  a real `call` here, and both bodies in one TU would let /Ob2 inline it.
//  Same rule the rest of src/ follows - a separate TU rather than a
//  __declspec(noinline) the shipping build never had.
//
//  Reached from CGridVec2::ScaleToLength (Math/gridvec2scaletolength.cpp),
//  which is the only caller in this tree.
//============================================================================
#include "CFloat.h"

//  0x6F6EF750 - Pathfinding/pathsqrt.cpp.  Declared here rather than
//  including pathmove.h (which drags the whole movement pathfinder in);
//  the signature is character for character the one that header gives it,
//  because a re-declaration that differs in any part of the type is a
//  second mangled name and a link failure verify.py cannot see.
CFloat __fastcall CFloatSqrt(const CFloat& value);

CFloat __fastcall CFloatInvSqrt(const CFloat& value)
{
    return CFloatReciprocal(CFloatSqrt(value));
}
