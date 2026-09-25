//============================================================================
//  0x6F4079C0 - TSGrowableArray<T>::Assign(unsigned int, const T*) for a
//  two-byte element (Storm/storm.h's own ReleaseAndReserve()+Assign()
//  bodies, Containers/tsarray.inl) - the release-and-realloc-outright shape
//  documented there, followed by a raw per-element copy: two-byte `mov cx,
//  [src]` / `mov [dst], cx` pairs rather than a call, which is what a POD T
//  with no user-defined assignment compiles down to.  `retn 8`.
//
//  IDA's own type-descriptor label for the allocation tag is "aG_4", a
//  truncation this closure gives no fuller string for (unlike the
//  NTempest::C2Vector/C3Vector siblings below, whose own tags demangle
//  outright) - Misc/citemclosure_terrain_gates.cpp's own
//  ClearTerrainVectorFieldElement note already records the same tag beside
//  confirmed AvC2Vector/AvC3Vector ones without resolving it further.  So
//  the element identity here is a placeholder by size only, same
//  convention as Containers/computechunk21.cpp's own CHUNK12.
//============================================================================
#include "storm.h"
#define TSARRAY_TYPENAME "aG_4"
#include "tsarray.inl"

struct SUnresolvedField2 { char m_reservedUnknown[2]; };

template void
TSGrowableArray<SUnresolvedField2>::Assign(unsigned int, const SUnresolvedField2*);
