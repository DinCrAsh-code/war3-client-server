//============================================================================
//  0x6F4F9640 - TSGrowableArray<NTempest::C3Vector>::Assign(unsigned int,
//  const NTempest::C3Vector*), Storm/storm.h's ReleaseAndReserve()+Assign()
//  shape (Containers/tsarray.inl).  IDA's own label "aAvc3vectorNtem" is the
//  same truncation of ".?AVC3Vector@NTempest@@" that
//  Containers/vectorfree_c3vector.cpp's own destructor instantiation
//  reads at a different address neighbourhood (0x6F352F30) - a distinct
//  module reaching the same shipped class, so the type is redeclared here
//  rather than shared across translation units (one TU per module).  Each
//  element copies as three raw dword moves, matching the opaque 12-byte
//  placeholder both files use.  `retn 8`.
//============================================================================
#include "storm.h"
#define TSARRAY_TYPENAME ".?AVC3Vector@NTempest@@"
#include "tsarray.inl"

namespace NTempest
{
struct C3Vector
{
    char m_reservedUnknown[12];
};
}

template void
TSGrowableArray<NTempest::C3Vector>::Assign(unsigned int, const NTempest::C3Vector*);
