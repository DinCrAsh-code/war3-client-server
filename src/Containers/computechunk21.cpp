//============================================================================
//  0x6F3DEB30 - TSGrowableArray<T>::ComputeChunk for a twelve-byte element,
//  which is what makes the template's own `256 / sizeof(T)` ceiling 0x15.
//  A second instantiation of the same body the string table's bucket array
//  has at 0x6F4C41D0; the two differ only in which array they belong to.
//
//  The element type is not recoverable from the call sites in these dumps -
//  only its size is - so it is a named twelve-byte placeholder here rather
//  than a guess at what it holds.
//============================================================================
#include "storm.h"
#define TSARRAY_TYPENAME ".?AVCHUNK12@@"
#include "tsarray.inl"

//: Twelve bytes, which is all this instantiation's ComputeChunk depends on.
struct CHUNK12 { char m_bytes[12]; };

template unsigned int TSGrowableArray<CHUNK12>::ComputeChunk(unsigned int);
