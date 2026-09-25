//============================================================================
//  0x6F34C260 - TSGrowableArray<CImVector>::Assign(unsigned int, const
//  CImVector*).  `retn 8`.  Storm/storm.h's own declaration,
//  Containers/tsarray.inl's own body (ReleaseAndReserve followed by a
//  placement-copy and the count store) - see that file's own comment on
//  Assign() for why the per-element null test is placement new's, not the
//  source's.
//
//  CImVector itself (a packed colour, four bytes - Ntempest's own name,
//  ".?AVCImVector@NTempest@@") is never dereferenced by Assign() beyond
//  its size, so it stays a minimal four-byte placeholder rather than a
//  real field-by-field type - the same convention every other
//  TSGrowableArray/TSExplicitList instantiation in this repo uses for an
//  element type nothing in the reaching call tree actually opens.
//============================================================================
#include "storm.h"

struct CImVector { char m_bytes[4]; };

#define TSARRAY_TYPENAME ".?AVCImVector@NTempest@@"
#include "tsarray.inl"

template void TSGrowableArray<CImVector>::Assign(unsigned int, const CImVector*);
