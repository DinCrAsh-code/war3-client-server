//============================================================================
//  Two small, fully self-contained leaves found at BFS depth 6 out of
//  CItem's vtable slots 32/104 (see citemclosure_terrain_gates.cpp's own
//  header for the walk this is part of).  Both are pure - no vtable call,
//  no Storm allocation, no class identity to guess at - so they are real
//  reconstructions rather than thunks, unlike almost everything else this
//  session's frontier touched.
//============================================================================
#include "refcnt.h"

//----------------------------------------------------------------------------
//  0x6F6F19E0 - pack four bytes, sign-extended, into one big-endian int:
//  `(sbyte)p[3]`, shifted up and or'd with `(sbyte)p[2]`, `(sbyte)p[1]`,
//  `(sbyte)p[0]` in that order.  Reached from an "INDEX_%04d"-style lookup
//  key builder (FormatIndexLookupKey, citemclosure_terrain_gates.cpp) that
//  wants the packed tag back as a plain int rather than a 4-byte string.  No
//  owning class or caller-visible name for the packing itself; the
//  sign-extension survives every shift-and-or exactly the way the raw
//  bytes did in the shipped code, not just for the ASCII range.
//----------------------------------------------------------------------------
int __fastcall PackFourBytesBigEndianSigned(const char* p)
{
    int result = (signed char)p[3];
    result = (result << 8) | (signed char)p[2];
    result = (result << 8) | (signed char)p[1];
    result = (result << 8) | (signed char)p[0];
    return result;
}

//----------------------------------------------------------------------------
//  0x6F73B0D0 - release a `TRefCnt*` sitting at `+0x64` of a caller-supplied
//  element, if it is not already null, and clear the field.  `element`
//  arrives on the stack (`retn 4`), not in ecx: the shipped call site
//  (ReleaseTerrainGridMeshRefArray, citemclosure_terrain_gates.cpp) calls it
//  once per 0x94-byte array element rather than as one of the element's own
//  methods, so it stays a free function rather than a member of an unnamed
//  struct.
//----------------------------------------------------------------------------
void __stdcall ReleaseRefFieldAt0x64IfSet(void* element)
{
    TRefCnt** field = (TRefCnt**)((char*)element + 0x64);
    if (*field != 0)
    {
        (*field)->Release();
        *field = 0;
    }
}
