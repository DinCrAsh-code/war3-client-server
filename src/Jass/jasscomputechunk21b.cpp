//============================================================================
//  0x6F459470 - a second unattributed twelve-byte TSGrowableArray<T>::
//  ComputeChunk instantiation in the same JASS neighbourhood as
//  jasscomputechunk21a.cpp's 0x6F459350 - see that file's own note for why
//  each of these gets a distinct placeholder tag rather than a shared one.
//============================================================================
#include "storm.h"
#define TSARRAY_TYPENAME ".?AVJASSBUCKETCHUNK12B@@"
#include "tsarray.inl"

struct JASSBUCKETCHUNK12B { char m_bytes[12]; };

template unsigned int TSGrowableArray<JASSBUCKETCHUNK12B>::ComputeChunk(unsigned int);
