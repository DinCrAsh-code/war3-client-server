//============================================================================
//  The three shared sprite resources a model is set up from, and the
//  registries they are cached in.
//
//  Each is a reference-counted object built once per key and handed back on
//  every later request, and each registry is a separate shipped object with
//  its own Find/Insert pair.  The classes are named from the shipped
//  allocation tags, which are ground truth: 0x6F4ED140 tags its 0x2C bytes
//  ".\SprAnimList.cpp" line 0x58, and 0x6F4F00D0 / 0x6F4F0170 both tag
//  theirs ".\SprLinkTable.cpp", at lines 0x6B and 0x86 - two objects out of
//  one shipped module, in two registries, with two constructors.
//
//  Only the reference count is named.  The rest of each object is built and
//  read by code below this dump's depth cut, and inventing fields for it
//  would be inventing layout.
//============================================================================
#include "storm.h"
#include "allochashtable.h"

#ifndef SPRRESOURCE_H
#define SPRRESOURCE_H

//----------------------------------------------------------------------------
//  0x2C bytes each, with the count the acquire path bumps at +0x18.
//
//  The first 0x18 bytes are not padding: they are exactly a
//  TAllocatorHashTable node's own bookkeeping - m_hashval, then the two
//  TSLink<T> links the registry threads it onto (its own bucket chain and
//  the table-wide list), then the owned key string.  Confirmed against the
//  registries' own Find/Insert/Unlink bodies below: Find compares
//  `node->m_hashval` before it pays for the string, Insert stores through
//  `[node]` (m_hashval, +0) and reads/writes `[node+0x14]` (m_key) exactly
//  where SetKey's key-replace branch does, and Unlink's inlined
//  TSLink::Unlink() pair walks +4/+8 and +0xC/+0x10 - i.e. m_hashlink at
//  +4 and m_listlink at +0xC, each 8 bytes.
//----------------------------------------------------------------------------
struct SSprAnimList
{
    //  0x6F4ECC60 - the constructor proper.  A __thiscall member and not a
    //  free __fastcall: the shipped call site pushes *both* remaining
    //  arguments and the callee cleans eight, where __fastcall would put
    //  the first of them in edx and clean four - the exact arity mismatch
    //  tools/thunk_abi_audit.py exists to catch.
    //
    //  NOT reconstructed: the body builds a sorted TSGrowableArray of
    //  sub-records through half a dozen callees this closure does not
    //  otherwise reach (sub_6F50C370, sub_6F4EC9F0, sub_6F4E93B0,
    //  sub_6F00A740, sub_6F4EB100's qsort comparator) whose own field
    //  layouts are not established anywhere in this tree.  Left as a
    //  redirect in Widget/sprresource_thunks.cpp rather than guessed.
    void Construct(void* model, const char* path);

    unsigned int          m_hashval;     // +0x00
    TSLink<SSprAnimList>  m_hashlink;    // +0x04
    TSLink<SSprAnimList>  m_listlink;    // +0x0C
    const char*           m_key;         // +0x14
    unsigned int          m_refs;        // +0x18
    char                  m_reserved1C[0x10];
};

struct SSprLinkTable
{
    //  0x6F4EF810 - `retn 8`, same shape as SSprAnimList::Construct.  Also
    //  not reconstructed: it placement-registers itself through
    //  sub_6F5057A0 with a callback (sub_6F4EF720) this closure never
    //  reaches, so the object it hands that callback is unknown layout.
    void Construct(void* model, const char* path);

    unsigned int            m_hashval;   // +0x00
    TSLink<SSprLinkTable>   m_hashlink;  // +0x04
    TSLink<SSprLinkTable>   m_listlink;  // +0x0C
    const char*             m_key;       // +0x14
    unsigned int            m_refs;      // +0x18
    char                    m_reserved1C[0x10];
};

struct SSprLinkTableSecondary
{
    //  0x6F4EF980 - `retn 8`, same shape as SSprAnimList::Construct.  Same
    //  reason as SSprLinkTable::Construct: sub_6F501CD0 / sub_6F4EF8A0.
    void Construct(void* model, const char* path);

    unsigned int                     m_hashval;  // +0x00
    TSLink<SSprLinkTableSecondary>   m_hashlink; // +0x04
    TSLink<SSprLinkTableSecondary>   m_listlink; // +0x0C
    const char*                      m_key;      // +0x14
    unsigned int                     m_refs;     // +0x18
    char                             m_reserved1C[0x10];
};

//----------------------------------------------------------------------------
//  The registries.  Each is the object and not a pointer to one - every
//  shipped call site takes its address with `mov ecx, offset ...` - and
//  each carries a vftable, which is how the release path reaches its
//  free-a-node slot (slot 0 - see spranimlistrelease.cpp).
//
//  Field-for-field this is Containers/allochashtable.h's
//  `TAllocatorHashTable<T, HASHKEY_STRI, N>` - Find is its Ptr(), Insert is
//  its SetKey(), Unlink is its Unlink() - but written out here rather than
//  derived from it, for the reason Containers/hashtablep.inl's own header
//  comment gives: deriving one shipped symbol from another puts the body
//  under the base's mangled name, which is not the name these three
//  addresses carry (`SSprAnimListRegistry::Find`, not
//  `TAllocatorHashTable<...>::Ptr`). `N` (the derived allocator's growth
//  chunk) is never read by anything in this closure, so it is left
//  unspecified here - these three registries' own NewNode/CheckGrow/Rehash
//  are all below the dump's depth cut.
//----------------------------------------------------------------------------
struct SSprAnimListRegistry
{
    //  0x6F4ECF20 (`retn 4`) - HASHKEY_STRI::Compare is Storm ordinal 509
    //  (SStrnICmp), confirmed by the dump's own `call Storm_509`.
    SSprAnimList* Find(void* key);
    //  0x6F4ED0E0 (`retn 8`) - SetKey().  Void in the shipped code: the
    //  node pointer this returns to spranimlistacquire.cpp's `found` does
    //  not come back through eax (the dump's own retn leaves eax holding
    //  m_key, not the node), so the caller keeps `made` live across the
    //  call itself.  Declared `void` here rather than the pointer the
    //  header used to give it, to avoid emitting a `mov eax,...`/`ret` this
    //  address's own dump does not have.
    void Insert(SSprAnimList* made, void* key);
    //  0x6F4EB600 (`retn 4`) - take a node back out.  A member for the same
    //  reason the constructors are: the shipped call site pushes the node
    //  and puts only the table in ecx.
    void Unlink(SSprAnimList* node);

    //  0x6F4ECFB0 (`retn 8`) - Insert()'s one real call: pick the bucket,
    //  grow the table if the chains have got long, splice the node into
    //  that bucket and the table-wide list.  Below this dump's depth cut -
    //  it in turn calls this registry's own Initialize/CheckGrow, neither
    //  of which is reached from anywhere in this closure - so it stays a
    //  redirect in Widget/sprresource_thunks.cpp rather than a guess at
    //  those two bodies' field use.
    void LinkNode(SSprAnimList* ptr, unsigned int hashval);

    void*                                     m_vtable;   // +0x00
    TSExplicitList<SSprAnimList>              m_list;     // +0x04
    unsigned int                              m_probes;   // +0x10
    TSGrowableArray<TSExplicitList<SSprAnimList> > m_buckets; // +0x14
    unsigned int                              m_mask;     // +0x24
};

struct SSprLinkTableRegistry
{
    //  0x6F4EFB10 (`retn 4`) / 0x6F4F0010 (`retn 8`) - see
    //  SSprAnimListRegistry::Find/Insert above for the same reasoning.
    SSprLinkTable* Find(void* key);
    void Insert(SSprLinkTable* made, void* key);
    //  0x6F4EFDB0 (`retn 8`) - this registry's own LinkNode; see
    //  SSprAnimListRegistry::LinkNode above.
    void LinkNode(SSprLinkTable* ptr, unsigned int hashval);

    void*                                      m_vtable;
    TSExplicitList<SSprLinkTable>              m_list;
    unsigned int                               m_probes;
    TSGrowableArray<TSExplicitList<SSprLinkTable> > m_buckets;
    unsigned int                               m_mask;
};

struct SSprLinkTableSecondaryRegistry
{
    //  0x6F4EFBE0 (`retn 4`) / 0x6F4F0070 (`retn 8`) - same shape again.
    SSprLinkTableSecondary* Find(void* key);
    void Insert(SSprLinkTableSecondary* made, void* key);
    //  0x6F4EFE90 (`retn 8`) - this registry's own LinkNode; see
    //  SSprAnimListRegistry::LinkNode above.
    void LinkNode(SSprLinkTableSecondary* ptr, unsigned int hashval);

    void*                                               m_vtable;
    TSExplicitList<SSprLinkTableSecondary>              m_list;
    unsigned int                                        m_probes;
    TSGrowableArray<TSExplicitList<SSprLinkTableSecondary> > m_buckets;
    unsigned int                                        m_mask;
};

//: dword_6FAB80FC / dword_6FAB812C / dword_6FAB8154.
extern SSprAnimListRegistry           g_sprAnimLists;
extern SSprLinkTableRegistry          g_sprLinkTables;
extern SSprLinkTableSecondaryRegistry g_sprLinkTablesSecondary;

//  0x6F4ED140 / 0x6F4F00D0 / 0x6F4F0170 - Widget/spranimlistacquire.cpp and
//  Widget/sprlinktableacquire.cpp.
SSprAnimList* __fastcall AcquireSprAnimList(void* key, void* model,
                                            const char* path);
SSprLinkTable* __fastcall AcquireSprLinkTable(void* key, void* model,
                                              const char* path);
SSprLinkTableSecondary* __fastcall AcquireSprLinkTableSecondary(
                                              void* key, void* model,
                                              const char* path);

//  0x6F4EBA10 - Widget/spranimlistrelease.cpp.
void __fastcall ReleaseSprAnimList(SSprAnimList* list);

#endif
