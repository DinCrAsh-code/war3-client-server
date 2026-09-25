//============================================================================
//  0x6F24F3E0 - "how high is the world at (x, y)", the one call
//  JASS_GetLocationZ makes past the resolver.
//
//  Ground height first, then a second query that hands back a water height
//  for the same point; whichever is higher wins, and a water surface exactly
//  level with the ground counts as water.  The point handed to the second
//  query carries the ground height it has just computed as its z, which is
//  why the three-float block is built after the first call and not before.
//
//  The signature is not a guess: this function forwards its own two register
//  arguments straight into QueryGroundHeightHelper without touching either,
//  so it has that function's shape (item.cpp) - `__fastcall`, two register
//  words then two floats and a trailing int, `retn 0Ch`.  Its own caller
//  passes -1 and 0 for the register pair and 1 for the int.
//
//  Own translation unit: it calls the ground helper, the water query and
//  nothing else in this file.
//============================================================================
#include "fvec3.h"

//  0x6F0126F0 - item.cpp's own redirect; re-declared here, never re-thunked
//  (item_place.cpp does the same).
float __fastcall QueryGroundHeightHelper(int flag, void* outPtr,
                                         float x, float y, int unused);

//  0x6F00F5D0 - jasswaterheight.cpp.
int __fastcall QueryWaterHeightAt(const FVec3* point, float* outHeight);

float __fastcall QueryWorldHeightAt(int flag, void* outPtr,
                                    float x, float y, int mode)
{
    float water;
    FVec3 point;

    float ground = QueryGroundHeightHelper(flag, outPtr, x, y, mode);

    point.m_x = x;
    point.m_y = y;
    point.m_z = ground;

    //  Two nested tests and not one `&&`: the shipped code loads `ground`
    //  onto the stack *before* branching on the query's result, so the
    //  "no water here" exit shares the load with the comparison rather than
    //  sitting on its own path.
    //  Named and initialised to the ground height rather than returned from
    //  two exits: that is what keeps the value on the x87 stack across the
    //  branch, which is where the shipped code has it - `fld` of the ground
    //  height sits *above* the `je`, so the "no water here" exit shares it
    //  and the water branch's `fstp st(1)` overwrites it in place.  Two
    //  `return`s instead cost the shared load and re-read the slot at each
    //  exit (measured: 0.735 against 0.811).
    //  A max, spelled `a > b ? a : b` with the ground height named first,
    //  and every other spelling of the same max is worse.  The shipped
    //  `fld ground` sits above the `je`, so the "no water here" exit and the
    //  comparison share one load; naming `ground` first is what lets it stay
    //  as `fcom`'s implicit st(1) instead of being pushed a second time, and
    //  which operand is the ternary's *true* arm decides which of the two
    //  `fstp` forms falls through.  Measured, all against the same 37
    //  instructions: this 37/37; `water > ground ? water : ground` 35;
    //  `ground <= water ? water : ground` 30; `water < ground ? ground :
    //  water` 30; the same test as a named `float z` assigned in an `if` 30;
    //  three `return`s 28.  Ties go to the water, which is what `>` on the
    //  ground gives.
    if (QueryWaterHeightAt(&point, &water))
        return ground > water ? ground : water;

    return ground;
}
