//============================================================================
//  dword_6FAD13E4 - the UTF-16 side of sub_6F6BF900's wide/multi-byte
//  string-conversion scratch pair (not in this call tree; reached only as
//  these two globals' one caller, and their type is what its own
//  disassembly names).  TSGrowableArray<unsigned short>, growing 128
//  elements (256 bytes) at a time - Containers/tsarray.inl's own generic
//  body, one instantiation, the same way Config/preferencehashtable.cpp
//  instantiates it for a hash table's bucket array.
//============================================================================
#define TSARRAY_TYPENAME "aG_4"
#include "tsarray.inl"

//  0x6F4DBED0 - ComputeChunk: maxchunk = 256/sizeof(unsigned short) = 0x80,
//  the cap this build's disassembly writes into [ecx+0Ch] (m_chunk) and
//  returns once count reaches it.
template unsigned int TSGrowableArray<unsigned short>::ComputeChunk(unsigned int);
//  0x6F34C1E0 - SetAlloc: SMemReAlloc-in-place first, full alloc/copy/free
//  fallback behind it, the element copy a straight `mov cx,[...]` word move
//  rather than a placement-new loop (unsigned short has a trivial dtor).
template void TSGrowableArray<unsigned short>::SetAlloc(unsigned int);
