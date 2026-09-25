//============================================================================
//  Storm's two *string-keyed* chained hash tables.
//
//  Both are TSHashTable's layout with a key that is a `const char*` in the
//  node and a lookup that hashes the key itself rather than being handed a
//  hash value.  The class names are read out of the binary, not invented -
//  the constructors at 0x6F861800/0x6F861820/0x6F861840 stamp
//
//      ??_7?$TAllocatorHashTable@UPrefetchNode@@VHASHKEY_STRI@@$0IA@@@6B@
//      ??_7?$TAllocatorHashTable@UUncachableNode@@VHASHKEY_STRI@@$0CA@@@6B@
//      ??_7?$TSHashTable_P@UPrefetchListFile@@VHASHKEY_STRI_P@@@@6B@
//
//  into the three tables at 0x6FAB7A3C, 0x6FAB7A64 and 0x6FAB7A8C, i.e.
//
//      TAllocatorHashTable<PrefetchNode,   HASHKEY_STRI,   128>
//      TAllocatorHashTable<UncachableNode, HASHKEY_STRI,    32>
//      TSHashTable_P<PrefetchListFile,     HASHKEY_STRI_P>
//
//  (`$0IA@` is 0x80 = 128 and `$0CA@` is 0x20 = 32, in the nibble encoding
//  docs/msvc-vc8-idioms.md's "Reading MSVC's mangled names out of the
//  binary" describes.)  The game-string table at 0x6FACD214 is the same
//  `TAllocatorHashTable` template at <STRINGHASHNODE, HASHKEY_STR, 256>,
//  which is where this file's template came from - see Config/gamestrings.h.
//
//  The difference between the two templates is the *key storage*.
//  TAllocatorHashTable's nodes own their key string (SetKey duplicates it
//  through Storm ordinal 507 and frees the old one); TSHashTable_P keeps a
//  pool of 8 KB blocks threaded through their own first dword and bump-
//  allocates every key out of it, which is what the four extra fields at
//  +0x28..+0x34 are - the "_P" in the shipped name.
//
//  Neither is declared with a single `virtual`, for the reason
//  Config/gamestrings.h already gives: this build has no business emitting a
//  vtable for a class it only ever reads through.  The two slots the shipped
//  code does reach - 0 (free one node) and 1 (allocate one node) - are
//  hand-written dispatches through m_vtable, the shape
//  tools/vtable_dispatch_audit.py judges.
//============================================================================
#ifndef ALLOCHASHTABLE_H
#define ALLOCHASHTABLE_H

#include "storm.h"

//  Storm.dll by ordinal.  508 is the bounded case-*sensitive* compare, 509
//  (SStrnICmp) the case-insensitive one, 590 the string hash the tables key
//  on, 501 a bounded copy, 506 a length and 507 a heap-duplicating strdup.
//  Declared as ordinary C++ rather than `extern "C"`, like storm.h's SMem*:
//  the pipeline recovers readable names from the comment MASM writes beside
//  a mangled EXTRN, and an undecorated C symbol has none.
int          __stdcall Storm_508(const char* a, const char* b,
                                 unsigned int count);
int          __stdcall SStrnICmp(const char* a, const char* b,
                                 unsigned int count);       // ordinal 509
unsigned int __stdcall Storm_590(const char* s);
void         __stdcall Storm_501(char* dest, const char* src,
                                 unsigned int maxChars);
unsigned int __stdcall Storm_506(const char* s);
char*        __stdcall Storm_507(const char* text, const char* file, int line);

//----------------------------------------------------------------------------
//  The three key policies.  Nothing in this closure touches a key object's
//  own members - the node holds the string and the table hashes it - so all
//  each type contributes is which of Storm's two compares the walk uses.
//  Written as a static rather than a member so no key object is ever
//  materialised: the shipped walks call the compare on two `const char*`s.
//----------------------------------------------------------------------------
class HASHKEY_STR
{
public:
    static int Compare(const char* a, const char* b)
    { return Storm_508(a, b, 0x7FFFFFFF); }
};

class HASHKEY_STRI
{
public:
    static int Compare(const char* a, const char* b)
    { return SStrnICmp(a, b, 0x7FFFFFFF); }
};

class HASHKEY_STRI_P
{
public:
    static int Compare(const char* a, const char* b)
    { return SStrnICmp(a, b, 0x7FFFFFFF); }
};

//----------------------------------------------------------------------------
//  TAllocatorHashTable<T, K, BLOCKSIZE>.
//
//  BLOCKSIZE is how many nodes the *derived* class's allocator grows by; the
//  table itself never reads it, which is why every member below compiles the
//  same for 32, 128 and 256 and why the three instantiations are three
//  symbols with one body.  Nodes come from and go back to vtable slots 1 and
//  0 respectively.
//----------------------------------------------------------------------------
template <class T, class K, int BLOCKSIZE>
class TAllocatorHashTable
{
public:
    //  0x6F4BF040 / 0x6F4BF0D0 / 0x6F5C8ED0 - hash the key, walk the bucket
    //  from the back, compare the cheap hash before paying for the string.
    T* Ptr(const char* key);

    //  0x6F45ADA0 / 0x6F45AE80 (the JASS compiler's own type-name table -
    //  src/Jass/jasssymtables.cpp) - Ptr() with one extra shortcut ahead of
    //  the string compare: a raw pointer-equality test against the node's
    //  own key.  Real for a table whose keys are mostly re-looked-up by the
    //  same interned literal the parser handed it the first time ("handle",
    //  the other builtin type names) - the string compare only has to run
    //  for a genuine miss on that fast path, not for every probe.
    T* PtrInterned(const char* key);

    //  0x6F45B000 (the JASS compiler's type-id table - src/Jass/
    //  jasssymtables.cpp) - a hashval-only lookup over the same table
    //  layout, the way Containers/tshash.inl's TSHashTable<T,K>::
    //  PtrHashOnly is: the caller still pushes a key argument (`retn 8`,
    //  the same two-argument shape TSHashTable::Ptr has), it is provably
    //  never read here, and node->m_hashval alone decides the match.  A
    //  member of this template rather than of TSHashTable itself because
    //  every other member this table needs (Initialize/CheckGrow/Rehash/
    //  NewNode) is TAllocatorHashTable's own non-virtual dispatch, not
    //  TSHashTable's `virtual AllocNode`.
    T* PtrHashOnly(unsigned int hashval, const K& key);

    //  0x6F4C01F0 / 0x6F4C0250 - four buckets, each told where the node's
    //  bucket link sits.
    void Initialize();

    //  0x6F4C0C30 / 0x6F4C0CD0 - grow if this bucket's chain has got long.
    int CheckGrow(unsigned int index);

    //  0x6F4C02B0 / 0x6F4C0480 - resize the bucket array and relink.
    void Rehash(unsigned int newcount);

    //  0x6F4C0EF0 / 0x6F4C0F90 - allocate a node through slot 1 and put it
    //  at the head of both its bucket and the table-wide list.
    T* NewNode(unsigned int hashval, void* arg1, void* arg2);

    //  0x6F4C0E10 - the same splice for a node that already exists.
    void LinkNode(T* ptr, unsigned int hashval);

    //  0x6F4C1210 - move a node to the bucket its new key hashes to and
    //  take a private copy of that key.
    void SetKey(T* ptr, const char* key);

    //  0x6F454510 (Jass/jassdeclare.cpp) - NewNode plus SetKey's own
    //  free-then-strdup key logic, folded into one call: unlike SetKey,
    //  the hash is handed in rather than recomputed (the node is known
    //  fresh, so there is nothing to unlink/relink) and NewNode's own
    //  `arg1`/`arg2` are threaded straight through.
    T* NewNodeWithKey(unsigned int hash, const char* key, void* arg1,
                      void* arg2);

    //  0x6F454820 / 0x6F4548B0 (Jass/jassdeclare.cpp) - a plain
    //  hash-and-forward `Declare` straight on the table itself, distinct
    //  from a *scope's* own Declare (JASSSCOPE::Declare,
    //  Jass/jasssymtables.h) which looks a name up first and only inserts
    //  on a miss.
    T* Declare(const char* name, void* arg1, void* arg2);

    //  0x6F4BD640 / 0x6F4CCA70 - take a node out of both lists.
    void Unlink(T* ptr);

    //  0x6F4BE220 - Unlink plus slot 0, answering the node the table-wide
    //  walk would continue from.
    T* Delete(T* ptr);

    //  Where the bucket list's link sits inside a node: 4 for a plain node,
    //  0xC for one that carries a TRefCnt base.  A constant the compiler
    //  folds, recomputed per instantiation.
    static int HashLinkOffset()
    { return (int)(unsigned int)&((T*)0)->m_hashlink; }

    void*                                m_vtable;   // +0x00
    TSExplicitList<T>                    m_list;     // +0x04, every node
    unsigned int                         m_probes;   // +0x10
    TSGrowableArray<TSExplicitList<T> >  m_buckets;  // +0x14
    unsigned int                         m_mask;     // +0x24, -1 while unbuilt
};

//----------------------------------------------------------------------------
//  TSHashTable_P<T, K> - the same table with a key *pool*.
//----------------------------------------------------------------------------
template <class T, class K>
class TSHashTable_P
{
public:
    //  0x6F4BF160.
    T* Ptr(const char* key);
    //  0x6F4C0650.
    void Initialize();
    //  0x6F4C0D70.
    int CheckGrow(unsigned int index);
    //  0x6F4C06B0.
    void Rehash(unsigned int newcount);
    //  0x6F4C1030.
    T* NewNode(unsigned int hashval, void* arg1, void* arg2);
    //  0x6F4C1510 - NewNode with the key copied into the pool first.
    T* New(const char* key, void* arg1, void* arg2);
    //  0x6F4BCA30 - bump-allocate one string out of the pool.
    char* AllocString(const char* text);

    static int HashLinkOffset()
    { return (int)(unsigned int)&((T*)0)->m_hashlink; }

    void*                                m_vtable;       // +0x00
    TSExplicitList<T>                    m_list;         // +0x04
    unsigned int                         m_probes;       // +0x10
    TSGrowableArray<TSExplicitList<T> >  m_buckets;      // +0x14
    unsigned int                         m_mask;         // +0x24
    char*                                m_firstBlock;   // +0x28
    char*                                m_currentBlock; // +0x2C
    unsigned int                         m_blockUsed;    // +0x30
    unsigned int                         m_stringCount;  // +0x34
};

#endif
