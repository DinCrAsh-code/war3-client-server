//============================================================================
//  TSGrowableArray<T*> for an opaque, pointer-sized element - the compiler's
//  own node-argument growth (sub_6F45D720, the AST/bytecode dispatcher this
//  batch left pending - see jassvm_callee_worklist.json).  Its RTTI
//  allocation tag prints as `aPak` in the dump, IDA's own auto-name for a
//  short string this call tree never spells out in full.  Spelled as its
//  own opaque type (SJassPakElem) rather than reusing
//  Containers/ptrarray.cpp's own TSGrowableArray<void*> instantiation: the
//  RTTI tag string is a file-local static, not part of a template
//  instantiation's mangled name, so re-instantiating T=void* here would be
//  the identical linker symbol ptrarray.cpp already defines - one body,
//  two definitions.
//
//  0x6F449250 ComputeChunk (cap 0x40 = 256/sizeof(SJassPakElem*)), 0x6F4492B0
//  SetAlloc.
//============================================================================
struct SJassPakElem;

#define TSARRAY_TYPENAME ".PAK@@"
#include "tsarray.inl"

template unsigned int TSGrowableArray<SJassPakElem*>::ComputeChunk(unsigned int);
template void TSGrowableArray<SJassPakElem*>::SetAlloc(unsigned int);

//  0x6F459B90 - Grow() (Containers/tsarray.inl), out of line for this
//  instantiation too: grow by one (through the same ComputeChunk/SetAlloc
//  pair above when the array is full) and hand back the fresh, uninitialised
//  slot.
template SJassPakElem** TSGrowableArray<SJassPakElem*>::Grow();
