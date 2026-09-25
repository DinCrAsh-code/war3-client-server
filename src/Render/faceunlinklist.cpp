//============================================================================
//  0x6F7BAE30 - one more TSExplicitList<T>::UnlinkAll instantiation, called
//  from FACECACHEENTRY's own destructor (0x6F7C5090, thunked in
//  Render/facecacheentry_thunks.cpp) against the list embedded at its own
//  +0x08.  Byte-identical to every other instantiation of this generic
//  Containers/tslist.inl body regardless of T, so the element type does not
//  have to be known to reconstruct it - only a name for it does, and
//  nothing in this closure's own dump reads a field of whatever it really
//  is.
//============================================================================
#include "storm.h"

struct SFaceCacheAuxNode;   // element type not confirmed by this closure

#include "tslist.inl"

template void TSExplicitList<SFaceCacheAuxNode>::UnlinkAll();
