//============================================================================
//  0x6F4D4F20 - TSFixedArray<CBaseManaged*>::SetAlloc.
//
//  CDataMgr's own property table (the pointer-array field CDataMgr's ctor
//  zeroes at +0x10, then immediately sizes to nine slots through this same
//  function - sub_6F4D85D0, out of this batch's scope) is a plain
//  TSFixedArray, not TSGrowableArray: it takes no ComputeChunk detour and
//  its own body has no shrink-side release loop, because CBaseManaged* is a
//  raw, non-owning pointer with a trivial destructor - the array does not
//  own what it points at.
//
//  The Storm allocation tag is ".PAVCBaseManaged@@" - `class CBaseManaged*`
//  by RTTI name - read directly off the immediate this function's own
//  `push offset aPavcbasemanage` sites carry.
//============================================================================
class CBaseManaged;    // opaque: only ever held here as a raw pointer.

#define TSFIXEDARRAY_TYPENAME ".PAVCBaseManaged@@"
#include "tsfixedarray.inl"

template void TSFixedArray<CBaseManaged*>::SetAlloc(unsigned int);
