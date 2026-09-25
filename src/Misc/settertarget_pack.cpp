//============================================================================
//  A shared helper (0x6F6FDE70) and five thin callers (0x6F6FE330-0x6F6FE3B0)
//  in the same ISetterTarget address neighbourhood as everything else in
//  this file's siblings: each caller supplies one block of four
//  consecutive field ids, the helper queries all four through SetFieldA
//  and packs the results into one dword, written through an output
//  pointer the caller then hands back unchanged.
//
//  The five blocks tile ids 0x1C-0x2F with no gaps (0x1C-1F, 0x20-23,
//  0x24-27, 0x28-2B, 0x2C-2F) - almost certainly five instances of the
//  same four-component structure (a color, most likely, given the packed
//  byte order below) rather than five unrelated groups.
//============================================================================
#include "settertarget.h"

//----------------------------------------------------------------------------
//  0x6F6FE350 - ids 0x1D, 0x1E, 0x1F queried first, 0x1C last (the block's
//  own base, in QueryFourFieldsPacked's "id0" slot).
//----------------------------------------------------------------------------
int* ISetterTarget::PackFourFields_Base0x1C(int* out, int value)
{
    QueryFourFieldsPacked(out, value, 0x1D, 0x1E, 0x1F, 0x1C);
    return out;
}

//----------------------------------------------------------------------------
//  0x6F6FE370 - ids 0x21, 0x22, 0x23, base 0x20.
//----------------------------------------------------------------------------
int* ISetterTarget::PackFourFields_Base0x20(int* out, int value)
{
    QueryFourFieldsPacked(out, value, 0x21, 0x22, 0x23, 0x20);
    return out;
}

//----------------------------------------------------------------------------
//  0x6F6FE390 - ids 0x25, 0x26, 0x27, base 0x24.
//----------------------------------------------------------------------------
int* ISetterTarget::PackFourFields_Base0x24(int* out, int value)
{
    QueryFourFieldsPacked(out, value, 0x25, 0x26, 0x27, 0x24);
    return out;
}

//----------------------------------------------------------------------------
//  0x6F6FE3B0 - ids 0x29, 0x2A, 0x2B, base 0x28.
//----------------------------------------------------------------------------
int* ISetterTarget::PackFourFields_Base0x28(int* out, int value)
{
    QueryFourFieldsPacked(out, value, 0x29, 0x2A, 0x2B, 0x28);
    return out;
}

//----------------------------------------------------------------------------
//  0x6F6FE330 - ids 0x2D, 0x2E, 0x2F, base 0x2C.
//----------------------------------------------------------------------------
int* ISetterTarget::PackFourFields_Base0x2C(int* out, int value)
{
    QueryFourFieldsPacked(out, value, 0x2D, 0x2E, 0x2F, 0x2C);
    return out;
}
