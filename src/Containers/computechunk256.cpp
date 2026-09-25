//============================================================================
//  0x6F467A50 - TSGrowableArray<T>::ComputeChunk for a *one-byte* element:
//  the template's own `256 / sizeof(T)` ceiling is 0x100 here, where the
//  string table's twelve-byte buckets make it 0x15.
//
//  Its own translation unit because it belongs to no instantiation this repo
//  has named - the element type is not recoverable from the two call sites
//  in these dumps, only its size is, and `char` is what reproduces the
//  constant.
//============================================================================
#include "storm.h"
#define TSARRAY_TYPENAME ".PAD"
#include "tsarray.inl"

template unsigned int TSGrowableArray<char>::ComputeChunk(unsigned int);
