//============================================================================
//  TSList<void, 0>::UnlinkAll.
//
//  The frame classes embed five of these lists as members (frame.h's +0xD0,
//  +0x118, +0x144, +0x174 and +0x180, and framethunks.h's SFrameSlot), so
//  every implicit destructor MSVC writes for a class holding one references
//  UnlinkAll.  The member is declared out of line - the shipped binary calls
//  it rather than inlining it, which is what tslist.inl's own header comment
//  records - so the declaration alone leaves an unresolved external that
//  nothing in verify.py can see: it compiles clean, it scores clean, and it
//  fails in the operator's build_mix.py.  tools/link_check.py is what found
//  it.
//
//  Its own translation unit for the same reason ostcp_sendqueue.cpp is:
//  naming the member is what makes MSVC emit the body, and the callers have
//  to keep seeing only the declaration.
//============================================================================
#include "frame.h"
#include "tsfixedlist.inl"

template void TSList<void, 0>::UnlinkAll();
