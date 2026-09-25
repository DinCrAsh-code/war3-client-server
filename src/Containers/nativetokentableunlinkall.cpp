//============================================================================
//  0x6F4E8C40 - TSExplicitList<NATIVETOKEN,0xC>::UnlinkAll, another
//  byte-identical instantiation of Containers/tslist.inl's generic walk
//  (see Widget/widgetlistunlinkall.cpp for why any element type produces
//  the same stream) reached through SNativeTokenTable::Construct
//  (0x6F4E9440, still a redirect - Containers/hashtablectors.cpp) calling
//  SetLinkOffset(0x0C) on its own all-nodes list during construction.
//
//  Not called from any C++ this repo writes - the constructor that reaches
//  it is a redirect - but scored the same way ExpandNibbleColor
//  (Storm/stormsingletonexpandtint.cpp) is for the same reason: it is a
//  real address in this closure's dump, so it gets a real body rather than
//  staying a gap in the worklist.
//============================================================================
#include "storm.h"
#include "tslist.inl"

//  NATIVETOKEN itself (CTokenizer's name->id record, Storm/tokenizer.h) is
//  never dereferenced by this walk - only its embedded TSLink, at a runtime
//  offset the object carries in m_linkoffset - so it stays incomplete here,
//  same convention as every other placeholder instantiation of this template.
struct NATIVETOKEN;

template void TSExplicitList<NATIVETOKEN, 0xC>::UnlinkAll();
