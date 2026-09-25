//============================================================================
//  0x6F459350 - TSGrowableArray<T>::ComputeChunk for a twelve-byte element
//  (256/12 = 0x15, the same ceiling every TSExplicitList<T> bucket array in
//  this JASS neighbourhood hits - JassTypeIdTable's own 0x6F4593B0,
//  JassTypeTable2's own 0x6F459410, Containers/computechunk21.cpp's own two
//  instantiations).  No call site in this closure's own dump reaches far
//  enough to name the owning table, so - same situation as that file's own
//  CHUNK12 - this is a uniquely-tagged twelve-byte placeholder purely to
//  give the instantiation its own mangled symbol, not a claim about what it
//  really holds.
//============================================================================
#include "storm.h"
#define TSARRAY_TYPENAME ".?AVJASSBUCKETCHUNK12A@@"
#include "tsarray.inl"

struct JASSBUCKETCHUNK12A { char m_bytes[12]; };

template unsigned int TSGrowableArray<JASSBUCKETCHUNK12A>::ComputeChunk(unsigned int);
