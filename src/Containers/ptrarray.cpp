//============================================================================
//  TSGrowableArray<void*> - a plain array of pointers with no per-element
//  construction to speak of: neither the copy done while growing nor the
//  destructor run while shrinking has anything to do (a pointer's "copy" is
//  just the four bytes, and there is nothing to release), so every loop the
//  generic TSGrowableArray<T> template writes around those two operations
//  optimises away and what is left is a plain SMemReAlloc/SMemAlloc/SMemFree
//  dance with a dword-at-a-time copy for the moved elements.
//
//  0x6F3A0480 ComputeChunk (cap 0x40 = 256/sizeof(void*)), 0x6F3A1AA0
//  SetAlloc, 0x6F3A3D70 SetCount.
//
//  0x6F3A3D20 - Grow<void*> (JASS VM closure, jassvm-core-A): reached from
//  JassInstance::ReleaseCompiledFunc's own String2HandleReg-table neighbour
//  (see jassnativehandlerelease.cpp) handing back a raw `void**` slot to
//  fill in - the same New()/Grow() split texthasharray.cpp's own family
//  already documents, just this session's own T=void* instantiation.
//============================================================================
#include "tsarray.inl"

template unsigned int TSGrowableArray<void*>::ComputeChunk(unsigned int);
template void TSGrowableArray<void*>::SetAlloc(unsigned int);
template void TSGrowableArray<void*>::SetCount(unsigned int);
template void** TSGrowableArray<void*>::Grow();
