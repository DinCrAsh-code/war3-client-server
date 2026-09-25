//============================================================================
//  0x6F4743C0 - CPathRef::SetWorldPosition: publish a new world position,
//  and hand `this` back so the call site reads as an assignment.
//
//  Four instructions of its own around a tail call into 0x6F474250, which
//  this repo already reconstructs as SHandleWithType::FlushedOriginDelta
//  (timesyncbounds.cpp).  The cast is deliberate and is *not* a claim that
//  the two are different classes: 0x6F4743A0 (CPathRef::SmartPtrToWorldVec3)
//  and 0x6F474250 are the same sixteen-byte record reached under two names
//  this repo already carried before this target, and re-spelling either one
//  would break the mangled name its existing callers link against.
//
//  The `1` is the shipped immediate, not a flag this reconstruction chose.
//
//  Own translation unit: the call to 0x6F474250 is a real call.
//============================================================================
#include "game.h"
#include "timesyncbounds.h"

CPathRef* CPathRef::SetWorldPosition(const CWorldVec3* pos)
{
    ((SHandleWithType*)this)->FlushedOriginDelta(pos, 1);
    return this;
}
