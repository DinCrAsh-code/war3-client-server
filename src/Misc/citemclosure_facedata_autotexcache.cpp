//============================================================================
//  0x6F7C28E0 - ReleaseAutoTextureCacheArray.  CItem vtable slots 32/104
//  closure, batch K continuation (depth 12-25+).
//
//  Destroys every element of an array of 16-byte entries, each carrying an
//  embedded `TSExplicitList<CHARCODEDESC>` at +0x04 (the leading four
//  bytes are read only as opaque padding here - nothing in this call tree
//  dereferences them).  The element type is not a guess: 0x6F7BECC0, the
//  UnlinkAll this loop calls, is the same address a concurrent FACEDATA
//  batch identified from real RTTI evidence as
//  `TSExplicitList<CHARCODEDESC>::UnlinkAll` (Render/facedata.h) - two
//  funcmap names for one address is a real conflict
//  (`gen_rename_from_funcmap.py` catches it), so this reuses that type
//  and that file's own instantiation rather than inventing a second,
//  unconfirmed placeholder element type and a duplicate one.
//
//  The per-element loop is `TSExplicitList<T>::~TSExplicitList()`'s own
//  body - `UnlinkAll()` (the real out-of-line call above) followed by
//  `m_terminator.Unlink()` transcribed inline, matching tslist.inl's own
//  destructor exactly - written out by hand here rather than through the
//  class because the array holds the *embedded* list offset by 4 bytes
//  into each entry, not a bare `TSExplicitList<T>[]`.
//
//  The Storm_403 (SMemFree) tag reads "AutexTureCache" in IDA's own
//  string-literal naming (the identifier a type descriptor's mangled name
//  reduces to after non-alphanumeric characters are stripped) - a
//  *different* array from Render/facedata.h's own three CHARCODEDESC/
//  GLYPHBITMAPDATA/KERNNODE tables (this one is a plain array of embedded
//  lists, not a TSHashTable-shaped object), reusing the node type only.
//
//  Deliberately does *not* include Containers/tslist.inl: storm.h's own
//  `~TSExplicitList()` is inline and visible from storm.h alone, but its
//  call to `UnlinkAll()` only stays a real out-of-line call - matching the
//  shipped stream's own `call sub_6F7BECC0` - so long as `UnlinkAll()`'s
//  body is not itself visible in this TU (with tslist.inl included here
//  too, /Ob2 inlines the whole loop into this one's own per-element
//  destructor call instead - measured, and it drops this function from a
//  plausible match to 10/48/0.225).
//============================================================================
#include "facedata.h"

//----------------------------------------------------------------------------
//  0x6F7C28E0 - ReleaseAutoTextureCacheArray: `self` is a pointer to a
//  {count@+4, data@+8} array header (the same shape TSFixedArray/
//  TSGrowableArray use for those two fields, but this call tree never
//  reaches a SetCount/SetAlloc pair for it, so it stays a raw pair rather
//  than a named template instantiation).
//----------------------------------------------------------------------------
struct SAutoTexCacheEntry
{
    char                              m_reserved00[4];
    TSExplicitList<CHARCODEDESC>      m_list;   // 0x04
};

void __fastcall ReleaseAutoTextureCacheArray(void* self)
{
    //  `self->data` (+0x08) is re-read every iteration rather than hoisted
    //  into a local, matching the shipped loop's own `mov eax,[edi+8]` at
    //  the top of each pass - the call to UnlinkAll() below is opaque to
    //  the optimiser, so it cannot prove `self` is unchanged across it.
    for (unsigned int i = 0; i < *(unsigned int*)((char*)self + 4); i++)
    {
        SAutoTexCacheEntry* elem =
            (SAutoTexCacheEntry*)((char*)*(void**)((char*)self + 8) + i * 16);
        elem->m_list.UnlinkAll();
        elem->m_list.m_terminator.Unlink();
    }

    void* data = *(void**)((char*)self + 8);
    if (data != 0)
        SMemFree(data, "AutoTextureCache", -2, 0);
}
