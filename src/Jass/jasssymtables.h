//============================================================================
//  The JASS compiler's own identifier tables - `.\Jass.cpp` (the __FILE__
//  string 0x6F455... functions in this neighbourhood hand Storm's
//  allocator).  Four `TAllocatorHashTable<T, HASHKEY_STR, N>` instantiations
//  live in this address range (0x6F449900-0x6F45D400), keyed by the same
//  layout Config/gamestrings.h's own string table uses (Containers/
//  allochashtable.h) - a vtable stamped by a constructor outside this
//  closure, buckets at +0x1C inside the growable array at +0x14, mask at
//  +0x24.
//
//  Two of the four tables' `Ptr()` (0x6F44EB80, 0x6F44CDA0) are the plain
//  walk allochash.inl already has; the other two (0x6F45ADA0, 0x6F45AE80)
//  splice in a raw pointer-equality test ahead of the string compare
//  (`PtrInterned`, added to the shared template in Containers/allochash.inl)
//  - real for a table whose keys are mostly re-looked-up by the very same
//  interned literal the parser handed it the first time.  0x6F454D04 stores
//  the literal "handle" straight into one such table's key field with no
//  Storm_507 duplication at all, which is what names these two the type
//  table below.
//
//  Node layout is not identical across the four: JASSNAMENODE and
//  JASSTYPENODE are Config/gamestrings.h's STRINGHASHNODE shape exactly
//  (m_hashval @0, m_key @0x14); JassNativeSlot's own fields sit four bytes
//  further out (m_hashval @4, m_key @0x18), which is what a lookup through
//  it reads at every one of those offsets and no others - one untouched
//  dword ahead of m_hashval that nothing in this closure reads or writes,
//  named only as a reserved slot rather than guessed at.
//============================================================================
#ifndef JASSSYMTABLES_H
#define JASSSYMTABLES_H

#include "allochashtable.h"

//----------------------------------------------------------------------------
//  0x6F44B2C0 (index accessor), 0x6F454930 (Declare) - the function/native
//  name table.  m_slot is the array index Declare() files the node under in
//  the parallel JassThreadLocal array (src/Jass/jassdeclare.cpp).
//----------------------------------------------------------------------------
struct JASSNAMENODE
{
    unsigned int         m_hashval;   // +0x00
    TSLink<JASSNAMENODE> m_hashlink;  // +0x04
    TSLink<JASSNAMENODE> m_listlink;  // +0x0C
    const char*          m_key;       // +0x14
    unsigned int          m_slot;      // +0x18
};

typedef TAllocatorHashTable<JASSNAMENODE, HASHKEY_STR, 64> JassNameTable;

//----------------------------------------------------------------------------
//  0x6F454E30 (Declare) - the variable/parameter table: a name, a type node
//  and a slot, which is what a declared JASS variable needs.  m_reserved00
//  is never read or written by anything this closure reaches; only its
//  presence (shifting every other field out by 4 bytes against
//  JASSNAMENODE) is confirmed.
//
//  Named JassNativeSlot, not something of this file's own invention:
//  jassvm-batch-3's Jass/jassnativeregtable.cpp reconstructs this table's
//  bucket array (0x6F449A30/0x6F44E8C0/0x6F44F8A0) and Rehash (0x6F452520)
//  under that name already, and says directly that 0x6F454E30/0x6F4542F0/
//  0x6F453890/0x6F451F50 - this file's own Ptr/NewNode/CheckGrow/Initialize
//  - are "jassvm-batch-2's own reconstruction of the same table". Reusing
//  their name is what lets both batches' `TSGrowableArray<TSExplicitList<
//  JassNativeSlot>>::ComputeChunk/SetAlloc/SetCount` instantiations bind to
//  the one symbol each of those three mangles to - see
//  jassvartablebuckets.cpp's own note on why the two TUs' local structs do
//  not have to agree on layout for that to hold. Their own comment guesses
//  m_hashval at offset 0 ("per tshash.inl"'s usual convention); this file's
//  own Ptr (0x6F44CDA0, `cmp [esi+4], ebx`) and Rehash's own dump
//  (0x6F452520, `mov eax,[ecx+4]` walking the parked list) both read it at
//  +0x04 instead, which is what this struct uses - a disagreement to
//  reconcile with that TU in a follow-up, not something this file could fix
//  by editing a TU it does not own.
//----------------------------------------------------------------------------
struct JassNativeSlot
{
    char                    m_reserved00[4]; // +0x00  unread in this closure
    unsigned int            m_hashval;       // +0x04
    TSLink<JassNativeSlot>  m_hashlink;      // +0x08
    TSLink<JassNativeSlot>  m_listlink;      // +0x10
    const char*             m_key;           // +0x18
    void*                   m_type;          // +0x1C  a JASSTYPENODE*
    char                    m_reserved20[4]; // +0x20  unread in this closure
    unsigned int            m_slot;          // +0x24
};

typedef TAllocatorHashTable<JassNativeSlot, HASHKEY_STR, 64> JassVarTable;

//----------------------------------------------------------------------------
//  0x6F45ADA0's table - the JASS type-name registry ("handle" and its
//  siblings).  Same shape as JASSNAMENODE; a distinct tag only so the two
//  instantiations mangle to two symbols (Containers/computechunk21.cpp's
//  CHUNK12 is the same trick).
//----------------------------------------------------------------------------
struct JASSTYPENODE
{
    unsigned int         m_hashval;   // +0x00
    TSLink<JASSTYPENODE> m_hashlink;  // +0x04
    TSLink<JASSTYPENODE> m_listlink;  // +0x0C
    const char*          m_key;       // +0x14
    JASSTYPENODE*         m_parent;    // +0x18  0x6F45D010
    JASSTYPENODE*         m_nextSibling;// +0x1C  0x6F45D010
    JASSTYPENODE*         m_firstChild; // +0x20  0x6F45D010
};

typedef TAllocatorHashTable<JASSTYPENODE, HASHKEY_STR, 64> JassTypeTable;

//----------------------------------------------------------------------------
//  0x6F45AE80's table.  Same layout as JASSTYPENODE again; a second tag for
//  the same reason.
//----------------------------------------------------------------------------
struct JASSTYPENODE2
{
    unsigned int          m_hashval;   // +0x00
    TSLink<JASSTYPENODE2> m_hashlink;  // +0x04
    TSLink<JASSTYPENODE2> m_listlink;  // +0x0C
    const char*           m_key;       // +0x14
    void*                  m_value;     // +0x18  0x6F45D300
    void*                  m_extra1C;   // +0x1C  0x6F45D300
};

typedef TAllocatorHashTable<JASSTYPENODE2, HASHKEY_STR, 64> JassTypeTable2;

//----------------------------------------------------------------------------
//  0x6F45B000's table - keyed by a raw hashval (a pointer or numeric id, not
//  a string: PtrHashOnly never calls Storm_590 and never touches m_key).
//  Reached from sub_6F45D300 right after a JASSTYPENODE2 lookup/insert, on a
//  key the caller already has in hand rather than one it has to hash - most
//  likely the reverse map from a JASS type id back to its declaration node.
//----------------------------------------------------------------------------
struct JASSTYPEIDNODE
{
    unsigned int            m_hashval;   // +0x00
    TSLink<JASSTYPEIDNODE>  m_hashlink;  // +0x04
    TSLink<JASSTYPEIDNODE>  m_listlink;  // +0x0C
    //  0x6F45D300 constructs an STStringField here (0x6F4C5CF0,
    //  Jass/jassnatives.h - already reconstructed as
    //  Misc/rcstringassigncstr.cpp) with the node's name; a plain byte run
    //  rather than the real type because this closure never reads it back.
    char                     m_ctorSub[0x04]; // +0x18
};

//  K is `unsigned int` here, not one of the string key policies: nothing in
//  this closure calls a member that ever touches K (PtrHashOnly's own key
//  argument is provably unread, and this batch does not reach SetKey), so
//  the raw hashval's own type is the only honest placeholder.
typedef TAllocatorHashTable<JASSTYPEIDNODE, unsigned int, 64> JassTypeIdTable;

//----------------------------------------------------------------------------
//  0x6F45D300's "<init>" branch - a plain node pool with no hashing at all:
//  every node goes on the one list's head, allocated straight through
//  Storm rather than through a table's own NewNode/vtable slot 1.  Reached
//  through a pointer this scope keeps beside its three hash tables.
//----------------------------------------------------------------------------
struct JASSCTORNODE
{
    TSLink<JASSCTORNODE> m_link;  // +0x00
    void*                 m_value; // +0x08
};

struct JASSCTORLIST
{
    void*                        m_vtable; // +0x00  unread in this closure
    TSExplicitList<JASSCTORNODE> m_list;   // +0x04
};

//----------------------------------------------------------------------------
//  0x6F454930's own "this" (src/Jass/jassdeclare.cpp) - a name table plus
//  the parallel "declared in order" array beside it.  Declared here rather
//  than only in jassdeclare.cpp because 0x6F454FF0
//  (src/Jass/jassloadfuncnames.cpp) also needs the type, through a pointer.
//----------------------------------------------------------------------------
struct JASSSCOPE
{
    TSGrowableArray<JASSNAMENODE*> m_bySlot;  // +0x00
    JassNameTable                  m_names;   // +0x10

    //  0x6F454930 - defined in jassdeclare.cpp.
    const char* Declare(const char* name, unsigned int slot);
};

#endif
