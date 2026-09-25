//============================================================================
//  0x6F00AAA0 - TSGrowableArray<void*>::~TSGrowableArray(), a second
//  instantiation over the same element type ptrarray.cpp's ComputeChunk/
//  SetAlloc/SetCount already cover at a different address: this call tree
//  reaches only the destructor of whatever array that is, so only the
//  destructor is instantiated here.  See storm.h/tsarray.inl - the
//  IDA-only "type descriptor name" string this passes to Storm is not
//  recoverable from a disassembly-only dump and does not affect the
//  generated code (ptrarray.cpp already relies on the same fact).
//============================================================================
#include "tsarray.inl"

template TSGrowableArray<void*>::~TSGrowableArray();
