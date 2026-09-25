//============================================================================
//  Four of the seven TSHashTable<T,K> members for one hash table used while
//  registering a JASS native: reached from the native-signature parser at
//  0x6F455110 through 0x6F4550C0/0x6F454E30/0x6F4542F0/0x6F453890/0x6F451F50,
//  none of which are jassvm-batch-3's - those five are jassvm-batch-2's own
//  reconstruction of the same table.  This TU supplies the other four:
//  Rehash (0x6F452520, which sub_6F453890's CheckGrow calls) and the three
//  TSGrowableArray<TSExplicitList<T>> members the bucket array itself needs
//  (ComputeChunk/SetAlloc/SetCount, T-independent code - see tsarray.inl -
//  so any correctly-sized placeholder T reproduces them byte for byte).
//
//  Nothing in this closure stamps a vtable for the node type or reaches a
//  key comparison, so the real element type is not recoverable from here:
//  Rehash only ever touches m_hashval (always offset 0, per tshash.inl) and
//  m_hashlink, whose offset - 0x08 - is the one hard fact this call tree
//  gives.  That is one field wider than the plain, non-virtual
//  TSHashObject<T,K> layout (hashval @0, hashlink @4) every other table in
//  this closure that shares the "168h"..."16Fh" allocation trail uses, so
//  there is a real 4-byte field between them that this closure never reads;
//  it stays reserved rather than guessed at.
//============================================================================
#include "storm.h"
#include "tsarray.inl"
#include "tshash.inl"
#include "tslist.inl"

struct JassNativeSlot
{
    unsigned int                m_hashval;      // 0x00
    char                        m_reserved04[4];// 0x04 - untouched by this closure
    TSLink<JassNativeSlot>      m_hashlink;      // 0x08
};

typedef JassNativeSlot NODE;
typedef TSExplicitList<NODE> BUCKET;

//  0x6F449A30
template unsigned int TSGrowableArray<BUCKET>::ComputeChunk(unsigned int);
//  0x6F44E8C0
template void TSGrowableArray<BUCKET>::SetAlloc(unsigned int);
//  0x6F44F8A0
template void TSGrowableArray<BUCKET>::SetCount(unsigned int);
//  0x6F452520
template void TSHashTable<NODE, HASHKEY_NONE>::Rehash(unsigned int);

//  UnlinkAll() this table's own Rehash calls (through SetLinkOffset(), a
//  Storm/storm.h inline) to empty the parked list before relinking it -
//  T-independent code, no address of its own in this closure (its callers
//  are jassvm-batch-2's own reconstruction of the rest of this table), so
//  this TU provides its own copy rather than leaving it unresolved.
template void TSExplicitList<NODE>::UnlinkAll();
