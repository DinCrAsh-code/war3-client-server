//============================================================================
//  TSGrowableArray<HashedString*> - the bucket array
//  TAllocatorHashTable<HashedString,HASHKEY_STR,1024>::TAllocatorHashTable
//  (0x6F453ED0, jasshashedstringtable.cpp) grows to size 1 on first
//  construction.  Trivial pointer copy/destroy, same as every other
//  TSGrowableArray<T*> instantiation in this closure.
//
//  The element type is not a guess: 0x6F448EA0 hands Storm the RTTI
//  descriptor name `.PAUHashedString@@` as its allocation tag.
//
//  0x6F448E40 ComputeChunk (cap 0x40 = 256/sizeof(HashedString*)), 0x6F448EA0
//  SetAlloc.
//============================================================================
struct HashedString;

#define TSARRAY_TYPENAME ".PAUHashedString@@"
#include "tsarray.inl"

template unsigned int TSGrowableArray<HashedString*>::ComputeChunk(unsigned int);
template void TSGrowableArray<HashedString*>::SetAlloc(unsigned int);
