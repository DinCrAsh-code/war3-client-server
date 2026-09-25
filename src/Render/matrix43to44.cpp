//============================================================================
//  0x6F5268D0 - FMATRIX44's converting constructor: widen a 4x3 into a 4x4
//  by adding the (0, 0, 0, 1) fourth column.
//
//  A constructor, not a free function, and the ABI is what says so: the
//  buffer arrives in ecx, the 4x3 as one stack dword (`retn 4`) and the
//  buffer comes back in eax off the entry `mov eax, ecx`.  fmatrix.h's three
//  free `__fastcall` routines all take their first operand in edx instead,
//  so no free spelling reaches this shape.
//
//  Every element moves through the x87 stack rather than as an integer,
//  which is what says the source copies `float`s and does not memcpy; the
//  three zeroes come off one `fldz` and the one off `fld1`.
//============================================================================
#include "renderdevice.h"

FMATRIX44::FMATRIX44(const FMATRIX43& m43)
{
    m[0]  = m43.m[0];
    m[1]  = m43.m[1];
    m[2]  = m43.m[2];
    m[3]  = 0.0f;
    m[4]  = m43.m[3];
    m[5]  = m43.m[4];
    m[6]  = m43.m[5];
    m[7]  = 0.0f;
    m[8]  = m43.m[6];
    m[9]  = m43.m[7];
    m[10] = m43.m[8];
    m[11] = 0.0f;
    m[12] = m43.m[9];
    m[13] = m43.m[10];
    m[14] = m43.m[11];
    m[15] = 1.0f;
}
