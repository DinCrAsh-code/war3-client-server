//============================================================================
//  JassVarTable's bucket array.  0x6F449A30 ComputeChunk, 0x6F44E8C0
//  SetAlloc, 0x6F44F8A0 SetCount and 0x6F452520 Rehash are jassvm-batch-3's
//  own worklist entries, already reconstructed in
//  src/Jass/jassnativeregtable.cpp against the same shipped table under the
//  name JassNativeSlot (their own file says so directly: "0x6F4550C0/
//  0x6F454E30/0x6F4542F0/0x6F453890/0x6F451F50 ... are jassvm-batch-2's own
//  reconstruction of the same table").  This TU supplies only what that one
//  does not: UnlinkAll(), which SetLinkOffset() calls the first time this
//  batch's own Initialize (0x6F451F50, jassvartable.cpp) sets a bucket's
//  link offset, and which has no address of its own in either batch's
//  worklist.
//
//  ComputeChunk/SetAlloc/SetCount's own generated code is identical no
//  matter which TU's `struct JassNativeSlot` the compiler saw: all three
//  depend only on `sizeof(TSExplicitList<JassNativeSlot>)`, which is always
//  12 bytes regardless of JassNativeSlot's own real size (TSExplicitList
//  never embeds a T, only a link), so the two TUs' otherwise-different
//  local definitions of the struct never have to agree for the linker to
//  be satisfied - only the type *name* has to match, for the mangled
//  symbol.
//============================================================================
#include "jasssymtables.h"
#include "tslist.inl"

template void TSExplicitList<JassNativeSlot>::UnlinkAll();
