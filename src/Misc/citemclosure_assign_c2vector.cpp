//============================================================================
//  0x6F50FBC0 - TSGrowableArray<NTempest::C2Vector>::Assign(unsigned int,
//  const NTempest::C2Vector*), the same ReleaseAndReserve()+Assign() shape
//  as citemclosure_assign_c3vector.cpp's own C3Vector instantiation.  IDA's
//  own label "aAvc2vectorNtem" demangles the same way that file's
//  "aAvc3vectorNtem" does, one component shorter -
//  Misc/citemclosure_terrain_gates.cpp's own ClearTerrainVectorFieldElement
//  note already names both tags side by side.  Each element copies as two
//  raw dword moves, matching an opaque 8-byte placeholder (most plausibly a
//  2-component vector, though nothing in this call tree reads through
//  one).  `retn 8`.
//============================================================================
#include "storm.h"
#define TSARRAY_TYPENAME ".?AVC2Vector@NTempest@@"
#include "tsarray.inl"

namespace NTempest
{
struct C2Vector
{
    char m_reservedUnknown[8];
};
}

template void
TSGrowableArray<NTempest::C2Vector>::Assign(unsigned int, const NTempest::C2Vector*);
