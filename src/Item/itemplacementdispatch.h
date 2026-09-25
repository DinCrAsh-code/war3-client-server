//============================================================================
//  See itemplacementdispatch.cpp.
//============================================================================
#ifndef ITEMPLACEMENTDISPATCH_H
#define ITEMPLACEMENTDISPATCH_H

#include "fvec3.h"

//  A 3x3 orientation matrix, row-major (9 floats) - the shape
//  0x6F2B6270 (BuildItemGroundOrientation, itemorientation.cpp) fills.
struct SOrientationMatrix
{
    float m[9];
};

void __fastcall SetItemPlacementOrientation(void* target, SOrientationMatrix m);
void __fastcall SetItemPlacementPosition(void* target, FVec3 pos);

//  0x6F4D3360 - a *data* write on the same placement sub-object, not
//  another vtable dispatch: set or clear `mask` (masked to the top byte
//  first) at the object's own +0x28.  Reached from
//  CSelectable::ClearPendingAndNotify (selectable.cpp), which is CItem's
//  own vtable[0x80] (item.cpp)'s first step.
void __fastcall SetItemPlacementFlagBits(void* target, unsigned int mask, int set);

#endif
