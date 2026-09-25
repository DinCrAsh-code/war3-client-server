//============================================================================
//  A hash table living inside the same FACEDATA glyph-cache object that
//  citemclosure_facedata_metrics.cpp and citemclosure_facedata_release.cpp
//  document (CItem vtable slots 32/104, this closure's deepest frontier
//  yet - depth 12-25+ off the item tooltip/text path).  Confirmed to be the
//  same generic Containers/tslist.inl, tsarray.inl and tshash.inl machinery
//  every other terrain/WEDB hash table in this closure already instantiates,
//  by the strongest evidence available short of RTTI: sub_6F7C7F90 and
//  sub_6F7C8770 (this table's own ComputeChunk/SetAlloc, reached from
//  SetCount below but not themselves in this session's worklist) are
//  instruction-for-instruction the bodies tsarray.inl already generates, and
//  the "AvTsexplicitli_72" Storm type tag their own Storm_401/403/405 calls
//  carry is the same family of tag every sibling instantiation in this repo
//  documents (see e.g. tools/funcmap.py's ReleaseTerrainGridMeshLinkedNodes
//  entry for "AvTsexplicitli_64").
//
//  HashLinkOffset() reads back as 0x0C from Initialize's own SetLinkOffset
//  call below, so the node's TSLink sits at +0x0C - three leading dwords
//  this call tree never dereferences by field, hence the reserved block.
//  Nothing here names the key policy either (Rehash and the table members
//  below never call Compare()), so K stays forward-declared and unnamed,
//  the same convention citemclosure_terrainhash4.cpp's own SPlaceholderKey4
//  documents.
//============================================================================
#include "storm.h"
#include "cstringrep.h"
#include "tslist.inl"

struct SFaceHashNode
{
    unsigned int             m_hashval;    // 0x00 - read by Rehash/Ptr below
    char                     m_reserved04[0x08];
    TSLink<SFaceHashNode>    m_hashlink;   // 0x0C
};

struct SFaceHashKey;

//  The Storm type tag this instantiation's Storm_401/403/405 calls carry
//  (see the file header above); not verified byte-for-byte against the
//  shipped mangled name, which the instruction score does not depend on
//  (both sides canonicalise a data reference to SYM either way).
#define TSARRAY_TYPENAME \
    ".?AV?$TSExplicitList@USFaceHashNode@@$0BA@@@"

#include "tsarray.inl"
#include "tshash.inl"

//----------------------------------------------------------------------------
//  0x6F7C8300 - TSExplicitList<SFaceHashNode>::UnlinkAll(), reached from
//  this table's own Initialize/Rehash below and from FACEDATA's own release
//  path (citemclosure_facedata_release.cpp).
//----------------------------------------------------------------------------
template void TSExplicitList<SFaceHashNode>::UnlinkAll();

//----------------------------------------------------------------------------
//  0x6F7C7F90 - this instantiation's own ComputeChunk, reached from SetCount
//  below.  Not itself part of this session's worklist (it fell to a sibling
//  batch's or an earlier session's address range), reconstructed here anyway
//  because SetCount cannot compile without it and the generic template
//  already has its exact body.
//----------------------------------------------------------------------------
template unsigned int
TSGrowableArray<TSExplicitList<SFaceHashNode> >::ComputeChunk(unsigned int);

//----------------------------------------------------------------------------
//  0x6F7C8770 - this instantiation's own SetAlloc, reached from SetCount
//  below.  Same note as ComputeChunk above.
//----------------------------------------------------------------------------
template void
TSGrowableArray<TSExplicitList<SFaceHashNode> >::SetAlloc(unsigned int);

//----------------------------------------------------------------------------
//  0x6F7C89A0 - the bucket array's SetCount, reached from Initialize and
//  Rehash below.
//----------------------------------------------------------------------------
template void
TSGrowableArray<TSExplicitList<SFaceHashNode> >::SetCount(unsigned int);

//----------------------------------------------------------------------------
//  0x6F7C8DB0 - first use of the table: four buckets, HashLinkOffset() 0x0C.
//----------------------------------------------------------------------------
template void TSHashTable<SFaceHashNode, SFaceHashKey>::Initialize();

//----------------------------------------------------------------------------
//  0x6F7C8FE0 - decide whether the table has to grow before another node
//  goes into the bucket at `index`.
//----------------------------------------------------------------------------
template int
TSHashTable<SFaceHashNode, SFaceHashKey>::CheckGrow(unsigned int);

//----------------------------------------------------------------------------
//  0x6F7C8E10 - resize the bucket array to `newcount` and put every node
//  back, reached from CheckGrow above.
//----------------------------------------------------------------------------
template void
TSHashTable<SFaceHashNode, SFaceHashKey>::Rehash(unsigned int);

//----------------------------------------------------------------------------
//  0x6F7C9080 - insert an already-allocated node into both of the table's
//  lists: its bucket (Initialize()-ing and CheckGrow()-ing the table first,
//  same as the generic template's own NewNode) and the table-wide m_list.
//  Not the generic NewNode()/AllocNode() pair - AllocNode is virtual and
//  would show as an indirect call, and this dispatches to neither: the
//  node arrives pre-built (`ptr`), so this is `LinkToHead` twice rather
//  than a real allocation.  Its own two stack arguments and `ecx`-only
//  register use (`retn 8`, no `edx`) are a plain non-static member
//  function's own thiscall shape, hence the derived class below rather
//  than a free function - `__fastcall` cannot produce "one stack arg
//  skipped past" the way a genuine member call does.  Deliberately not
//  added to the generic TSHashTable<T,K> in storm.h itself: nothing else
//  in this repo's other hash-table instantiations has this member, so it
//  is this table's own addition, not the shared template's.
//----------------------------------------------------------------------------
struct SFaceHashTableExt : TSHashTable<SFaceHashNode, SFaceHashKey>
{
    void InsertExistingNode(SFaceHashNode* ptr, unsigned int hashval);
    void InsertKeyedNode(void* node, const char* key);
};

void SFaceHashTableExt::InsertExistingNode(SFaceHashNode* ptr,
                                            unsigned int hashval)
{
    if (m_mask == 0xFFFFFFFF)
        Initialize();

    unsigned int index = hashval & m_mask;
    if (CheckGrow(index))
        index = hashval & m_mask;

    m_buckets[index].LinkToHead(ptr);
    m_list.LinkToHead(ptr);
}

//----------------------------------------------------------------------------
//  0x6F7C91B0 - hash `key`, insert `node` at that hash (InsertExistingNode
//  above), cache the hash at `node+8`, and - unless `node`'s own +0x1C
//  already holds this exact `key` pointer - free whatever string was there
//  (Storm_403/SMemFree) and duplicate `key` into it (Storm_507/SStrDupA),
//  the same "pointer identity guard, free-then-Storm_507" key-management
//  idiom CStringManager::Intern and the font/terrain caches' own Ptr()
//  callers already use throughout this repo, tagged with the same Storm
//  hash-module `aEDrive1TempBui` file and 0xA38/0xA39 line pair.
//----------------------------------------------------------------------------
void SFaceHashTableExt::InsertKeyedNode(void* node, const char* key)
{
    unsigned int hash = Storm_590(key);
    InsertExistingNode((SFaceHashNode*)node, hash);

    *(unsigned int*)((char*)node + 8) = hash;

    const char** slot = (const char**)((char*)node + 0x1C);
    if (*slot != key)
    {
        if (*slot != 0)
            SMemFree((void*)*slot, aEDrive1TempBui, 0xA38, 0);
        *slot = Storm_507(key, aEDrive1TempBui, 0xA39);
    }
}
