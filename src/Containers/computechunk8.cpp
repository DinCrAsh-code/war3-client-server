//============================================================================
//  0x6F50F500 - TSGrowableArray<T>::ComputeChunk for an eight-byte element,
//  which is what makes the template's own `256 / sizeof(T)` ceiling 0x20
//  (see computechunk21.cpp/computechunk256.cpp for the same body at other
//  element sizes).  The element type is not recoverable from this batch's
//  one caller (0x6F5057A0, whose own SSprLinkTable-adjacent registration
//  loop is out of this session's confident scope) - only its size is, so
//  it is a named eight-byte placeholder rather than a guess at what it
//  holds.
//============================================================================
#include "storm.h"
#define TSARRAY_TYPENAME ".?ACHUNK8@@"
#include "tsarray.inl"

struct CHUNK8 { char m_bytes[8]; };

template unsigned int TSGrowableArray<CHUNK8>::ComputeChunk(unsigned int);
