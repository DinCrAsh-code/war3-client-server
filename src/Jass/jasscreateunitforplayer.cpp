//============================================================================
//  0x6F24F1F0 - CreateUnitForPlayer: the thin wrapper JASS_CreateUnit
//  (jasscreateunit_native.cpp) actually calls.  It forwards every one of its
//  own arguments untouched to the real factory at 0x6F29F990 along with
//  three constants, and then, on success, plays one animation on the new
//  unit and throws the result away.
//
//  `playerIndex` and `unitId` never appear in this function's own body at
//  all - they arrive in ecx/edx and stay there across the call, which is
//  what makes the __fastcall declaration a reading rather than a guess.
//
//  The animation call is CUnit's vtable slot 35 (+0x8C), already
//  established and already reconstructed as CUnit::PlayAnimation
//  (unit_playanim.cpp, docs/targets/vtables/CUnit.txt) - so it is spelled
//  as the virtual call it is rather than as a hand-written slot dispatch,
//  which is also what keeps it out of vtable_dispatch_audit.py's "unknown"
//  bucket.  Its `float` result is discarded, which is the shipped
//  `fstp st` immediately after the call.
//
//  Its own translation unit: 0x6F24F1F0 and 0x6F3C5230 are a megabyte apart
//  and call each other for real, so they cannot share one.
//============================================================================
#include "jasscreateunit.h"
#include "unit.h"

CUnit* __fastcall CreateUnitForPlayer(int playerIndex, int unitId,
                                      const CFloat* posX,
                                      const CFloat* posY,
                                      const CFloat* radians)
{
    CUnit* unit = CreateUnitAtPosition(playerIndex, unitId, posX, posY,
                                       radians, 0x502, 1, -1);

    if (unit)
        unit->PlayAnimation(4, 0, 0);

    return unit;
}
