//============================================================================
//  A string-keyed hash table this closure's own dumps name outright: a
//  sibling constructor (0x6F70DAD0, not itself in this batch's target list -
//  it stamps `??_7?$TSHashTable@UWEDBEntryHash@@VHASHKEY_STRI@@@@6B@` into
//  the object under construction before calling this file's own Clear/
//  destructor pair) gives the element type its real shipped name,
//  WEDBEntryHash, and its key policy, HASHKEY_STRI - a World Editor
//  database entry, consistent with the "HDBENTRY"/"HDBFIELD" CDataAllocator
//  tags this closure's neighbouring functions push (out of this batch's
//  scope; see the sibling `sub_6F4C9xxx` family).  Three more instantiations
//  of the same generic Containers/tslist.inl, tsarray.inl and tshash.inl
//  machinery the terrain hash trio above uses: the all-nodes list's
//  UnlinkAll, the bucket array's own destructor, and Initialize() (the
//  four-bucket, mask-3 first-use case tshash.inl's own comment names).
//
//  Neither Initialize() nor UnlinkAll() nor the bucket array's destructor
//  ever dereferences WEDBEntryHash's own fields (Initialize only needs
//  HashLinkOffset()'s compile-time constant, folded to the literal `4` this
//  dump's own SetLinkOffset call writes - a plain node, no vtable, matching
//  Storm/storm.h's own HashLinkOffset() comment), so the placeholder stays
//  otherwise empty.
//============================================================================
#include "storm.h"
#include "allochashtable.h"
#include "tslist.inl"
#include "tsarray.inl"
#include "tshash.inl"

struct WEDBEntryHash { unsigned int m_hashval; TSLink<WEDBEntryHash> m_hashlink; };

//  0x6F7037B0 - the all-nodes list's UnlinkAll.
template void TSExplicitList<WEDBEntryHash>::UnlinkAll();

//  0x6F707AF0 - the bucket array's own destructor: every live bucket's
//  UnlinkAll (the call above), then the array's storage freed through
//  Storm - Containers/tsarray.inl's own ~TSGrowableArray().
template TSGrowableArray<TSExplicitList<WEDBEntryHash> >::~TSGrowableArray();

//  0x6F70B130 - a one-instruction `jmp sub_6F707AF0` in the shipped image:
//  a second entry point for the exact same destructor body above, not a
//  distinct implementation.  A C++ call to our own reconstruction would not
//  reliably compile down to the bare `jmp` the dump shows, so this is
//  addressed straight at the shipped body the same way any other redirect
//  is - the target is the destructor this file already reconstructs, so
//  the two addresses run identical code either way.
__declspec(naked) void TSGrowableArrayDtorAlias_WEDBEntryHash()
{
    __asm { mov eax, 06F707AF0h }
    __asm { jmp eax }
}

//  0x6F70F140 - Initialize(): four buckets, mask 3, every bucket told its
//  link offset.
template void TSHashTable<WEDBEntryHash, HASHKEY_STRI>::Initialize();

//  0x6F7103F0 - CheckGrow(unsigned int index): its own Rehash callee
//  (sub_6F70F370) is out of this batch's scope, but CheckGrow itself never
//  dereferences a node, so the instantiation is exact regardless.
template int TSHashTable<WEDBEntryHash, HASHKEY_STRI>::CheckGrow(unsigned int);

//  0x6F70D9E0 - the bucket array's own SetCount: what CheckGrow's Rehash
//  below calls to resize `m_buckets` before re-bucketing every parked node.
//  Containers/tsarray.inl's own SetCount.
template void TSGrowableArray<TSExplicitList<WEDBEntryHash> >::SetCount(unsigned int);

//  0x6F70F370 - CheckGrow's own Rehash callee named above (0x6F7103F0):
//  park every node off the old bucket array, resize it via SetCount, then
//  re-bucket each parked node by `hashval & m_mask`.  Containers/tshash.inl's
//  own Rehash.
template void TSHashTable<WEDBEntryHash, HASHKEY_STRI>::Rehash(unsigned int);
