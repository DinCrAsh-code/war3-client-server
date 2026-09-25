//============================================================================
//  0x6F020D30 - the angle new buildings are placed at, read from the game's
//  configuration and snapped to a quarter turn.
//
//  Misc/BuildingAngle is a value in degrees.  It is taken modulo 360, rounded
//  towards zero to a multiple of 90, and brought back into [0, 360) if the
//  configured value was negative - so any spelling of "north-east" lands on
//  exactly one of 0, 90, 180 or 270.
//============================================================================
#include "gamecontext.h"

CFloat __fastcall GetBuildingAngle()
{
    //  Named rather than nested, because the shipped code hands CFloatToInt
    //  the address of a stack slot (`lea ecx, [esp+4]`) instead of the pointer
    //  the lookup already returned in eax.
    CFloat configured = GetConfigFloat("Misc", "BuildingAngle", 0);

    int degrees = CFloatToInt(configured) % 360;
    //  Computed before the test, as the shipped code does: it is needed on the
    //  only path that does anything, and it is not worth a branch.
    int offAxis = degrees % 90;

    if (degrees != 0)
    {
        //  The inner test is redundant - subtracting a zero remainder changes
        //  nothing - but the shipped code branches around the subtraction, so
        //  the `if` is in the source.
        if (offAxis != 0)
            degrees -= offAxis;
        if (degrees < 0)
            degrees += 360;
    }

    return CFloatFromInt(degrees);
}
