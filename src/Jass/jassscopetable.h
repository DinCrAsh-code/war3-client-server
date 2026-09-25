//============================================================================
//  Jass2's compile-time symbol table - a fourth `TAllocatorHashTable<T,K,N>`
//  instantiation (Containers/allochashtable.h), string-keyed and
//  case-*sensitive* (Ptr's walk is `call Storm_508`, i.e. HASHKEY_STR - JASS
//  identifiers are case sensitive, unlike the three file-cache tables that
//  reuse this template with HASHKEY_STRI).
//
//  The node's own two fields this batch's closure touches are read straight
//  off the dump's displacements, the same way citemclosure_dbfieldhash.cpp's
//  SDatabaseFieldRecord was pinned: `cmp [eax],ecx`-style hash compare at
//  offset 0, and Initialize()'s `SetLinkOffset` constant of 4 for the bucket
//  link - i.e. HashLinkOffset() folds to `&((JassSymbolNode*)0)->m_hashlink`,
//  so the link sits right after the hash the way PrefetchNode's does too.
//  Nothing in this batch's closure reads the node past +0x18 (the key), so
//  whatever the parser actually hangs off a resolved symbol (its Node*, its
//  declared type) is not recovered here.
//
//  The owning object is a two-table parser/compile-state block: a flag at
//  +0x00 that is non-zero while compiling inside a function body, an
//  embedded *global* scope table at +0x04 and an embedded *local* scope
//  table at +0x2C - exactly `sizeof(TAllocatorHashTable<...>)` (0x28 bytes:
//  vtable + TSExplicitList<T> m_list + m_probes + TSGrowableArray m_buckets
//  + m_mask) further on, which is what pins the second table's offset.
//  sub_6F464420 (jassparseidentifier.cpp) is what switches between them on
//  the +0x00 flag.  Everything from +0x30 up to the +0xB8 "current position"
//  field every Node constructor copies out of this object is unrecovered.
//============================================================================
#ifndef JASSSCOPETABLE_H
#define JASSSCOPETABLE_H

#include "allochashtable.h"

class CDataStore;

//  One entry in a JASS scope (a declared variable or function name).
//
//  +0x18/+0x1C/+0x20 - jassvm-core-A's own reading (jassnativecalldispatch.cpp),
//  from a disjoint closure (JassInstance's own native-vs-user-defined
//  function-call dispatch, not this file's own parser-facing Declare/
//  Resolve members): +0x1C compared to 7 selects a native-function
//  dispatch, +0x18 compared to 6 selects a compiled-user-function release/
//  refcount path, and +0x20 is the payload each path hands onward (a
//  native's own declaration pointer, or a compiled function's own index).
//  Nothing in *this* closure confirms the two kind fields against
//  JassCompileContext::Declare's own writes, so treat them as this second
//  closure's own evidence layered onto the same offsets, not a widened
//  claim about what Declare itself sets.
struct JassSymbolNode
{
    unsigned int             m_hashval;    // +0x00
    TSLink<JassSymbolNode>   m_hashlink;   // +0x04
    TSLink<JassSymbolNode>   m_listlink;   // +0x0C
    const char*              m_key;        // +0x14
    int                      m_callKind;   // +0x18 - jassnativecalldispatch.cpp: ==6 selects the compiled-user-function path
    int                      m_declKind;   // +0x1C - jassnativecalldispatch.cpp: ==7 selects the native-function path
    void*                    m_declData;   // +0x20 - payload for whichever path above

    //  0x6F45B2E0 - `retn 4`.  jassnativecalldispatch.cpp.
    void* DispatchWithOwnDecl(class JassInstance* instance);
    //  0x6F459750 - `retn 8`.  jassnativecalldispatch.cpp.
    void* DispatchWithArg(class JassInstance* instance, void* arg);
    //  0x6F45C170 - `retn 8`.  jassnativecalldispatch.cpp.
    void* DispatchOrEvaluate(class JassInstance* instance, CDataStore* store);
    //  0x6F45B780 - `retn 0xC`.  jassnativecalldispatch.cpp.
    int UpdateBinding(int kind, void* data, class JassInstance* instance);
};

//  BLOCKSIZE is never read by the table itself (allochashtable.h) - it only
//  shapes the mangled name of a constructor this batch never reaches, so it
//  cannot be pinned from this closure and is left at a placeholder.
typedef TAllocatorHashTable<JassSymbolNode, HASHKEY_STR, 64>
    JassScopeTableBase;

//  A thin derived class rather than widening Containers/allochashtable.h:
//  Release() below is this batch's own closure, reached from `this` in ecx
//  plus one stack argument (thiscall's own shape - see
//  docs/msvc-vc8-idioms.md's "`__thiscall` on a free function: rejected by
//  this compiler"), and is not one of the template's own members every
//  other instantiation in this repo shares.
class JassScopeTable : public JassScopeTableBase
{
public:
    //  0x6F457630 - jasssymboltablerelease.cpp.
    void Release(int keepInBuckets);
};

//  The parser/compile-state object both scope tables live in
//  (`JassCompileContext`) is declared in jassnodes.h, not here: every Node
//  constructor also needs it (for the +0xB8 pool pointer), and jassnodes.h
//  is the header both batches' own dumps already converge on for that
//  object, so a second, competing struct here would just be the same
//  fields under a different name.

#endif
