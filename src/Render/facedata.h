//============================================================================
//  FACEDATA - one loaded TrueType font face, allocated from Storm under the
//  "HFACE" tag (sub_6F7C9210, the face-cache lookup-or-create this closure
//  does not fully reconstruct) and stamped with `??_7FACEDATA@@6B@`
//  (0x6F985D38, read via tools/ida_query.py bytes at the ctor's own `mov
//  dword ptr [eax], offset ...` instruction - the RTTI name is the shipped
//  binary's own).
//
//  Three `TSHashTable<T,HASHKEY_NONE resp. KERNINGHASHKEY>`-shaped caches
//  hang off a bigger face-cache record (below) at +0x1C/+0x44/+0x6C:
//
//    ??_7?$TSHashTable@UCHARCODEDESC@@VHASHKEY_NONE@@@@6B@      0x6F985CB4
//    ??_7?$TSHashTable@UGLYPHBITMAPDATA@@VHASHKEY_NONE@@@@6B@   0x6F985CDC
//    ??_7?$TSHashTable@UKERNNODE@@VKERNINGHASHKEY@@@@6B@        0x6F985CF0
//
//  All three RTTI names, and all three literal addresses, read the same way
//  clayer.h's SHORTCUTREGTABLE already established: modelled as a plain
//  struct rather than as an instantiation of storm.h's TSHashTable<T,K>,
//  because that template declares AllocNode "declared, never defined" -
//  instantiating any out-of-line *virtual* member of it (the destructor,
//  and Clear() below) would force this build to emit the class's own
//  vtable COMDAT, which needs a real address for AllocNode's slot this repo
//  never gives it.  A plain struct with a literal vtable pointer sidesteps
//  the problem entirely: nothing here is declared `virtual`, so the
//  compiler never has to build one.
//
//  CHARCODEDESC/GLYPHBITMAPDATA/KERNNODE are themselves opaque here - this
//  closure never reads a field of any of them, only the table plumbing
//  around them (TSExplicitList<T>::UnlinkAll and its callers), so each is
//  an incomplete forward declaration.
//============================================================================
#ifndef FACEDATA_H
#define FACEDATA_H

#include "storm.h"

struct CHARCODEDESC;
struct GLYPHBITMAPDATA;
struct KERNNODE;

//: 0x6F985D38 - FACEDATA's own vtable, written as a literal for the reason
//: this file's own header comment gives (and the one Frame/clayer.h's
//: kShortcutRegTableVftable already gives in full).
const unsigned int kFaceDataVftable = 0x6F985D38u;

const unsigned int kCharCodeDescTableVftable  = 0x6F985CB4u;
const unsigned int kGlyphBitmapTableVftable   = 0x6F985CDCu;
const unsigned int kKernNodeTableVftable      = 0x6F985CF0u;

//----------------------------------------------------------------------------
//  One `TSHashTable<T,K>`-shaped bucket table, field for field with
//  storm.h's own template (Frame/clayer.h's SHORTCUTREGTABLE is the
//  precedent for the whole shape).
//----------------------------------------------------------------------------
struct CHARCODEDESC_TABLE
{
    CHARCODEDESC_TABLE();
    ~CHARCODEDESC_TABLE();
    void Clear(unsigned int freeNodes);

    void*                                   m_vtable;   // +0x00
    TSExplicitList<CHARCODEDESC>            m_list;     // +0x04
    unsigned int                            m_probes;   // +0x10
    TSGrowableArray<TSExplicitList<CHARCODEDESC> >
                                             m_buckets;  // +0x14
    unsigned int                            m_mask;     // +0x24
};

struct GLYPHBITMAP_TABLE
{
    GLYPHBITMAP_TABLE();
    ~GLYPHBITMAP_TABLE();
    void Clear(unsigned int freeNodes);

    void*                                   m_vtable;   // +0x00
    TSExplicitList<GLYPHBITMAPDATA>         m_list;     // +0x04
    unsigned int                            m_probes;   // +0x10
    TSGrowableArray<TSExplicitList<GLYPHBITMAPDATA> >
                                             m_buckets;  // +0x14
    unsigned int                            m_mask;     // +0x24
};

struct KERNNODE_TABLE
{
    KERNNODE_TABLE();
    ~KERNNODE_TABLE();
    void Clear(unsigned int freeNodes);

    void*                                   m_vtable;   // +0x00
    TSExplicitList<KERNNODE>                m_list;     // +0x04
    unsigned int                            m_probes;   // +0x10
    TSGrowableArray<TSExplicitList<KERNNODE> >
                                             m_buckets;  // +0x14
    unsigned int                            m_mask;     // +0x24
};

//----------------------------------------------------------------------------
//  FACEDATA - the small (0x2C-byte) per-face record 0x6F7C9210's face-cache
//  lookup allocates from Storm under the "HFACE" tag and stamps with
//  `??_7FACEDATA@@6B@` (kFaceDataVftable).  Only the constructor's own eight
//  touched offsets are named; the two fields 0x6F7C9210 fills in right after
//  construction (+0x20, +0x24) are read there, not here.
//----------------------------------------------------------------------------
struct FACEDATA
{
    FACEDATA();

    void*        m_vtable;          // +0x00
    unsigned int m_reserved04;      // +0x04
    unsigned int m_reserved08;      // +0x08  untouched by the ctor
    unsigned int m_reserved0C;      // +0x0C
    unsigned int m_reserved10;      // +0x10
    unsigned int m_reserved14;      // +0x14
    unsigned int m_reserved18;      // +0x18
    unsigned int m_reserved1C;      // +0x1C
    unsigned int m_reserved20;      // +0x20  filled by the caller (0x6F7C9210)
    unsigned int m_reserved24;      // +0x24  filled by the caller (0x6F7C9210)
};

//----------------------------------------------------------------------------
//  The much bigger per-loaded-face cache entry the three bucket tables
//  above actually live in (CHARCODEDESC_TABLE at +0x1C, GLYPHBITMAP_TABLE at
//  +0x44, a KERNNODE_TABLE-shaped table at +0x6C that 0x6F7C4FC0's own ctor
//  re-stamps as a `TAllocatorHashTable<KERNNODE,KERNINGHASHKEY,128>` right
//  after constructing it - a derived class restamping its own vtable over a
//  base's, the same idiom CLayer's own CObserver base gets).  No vtable of
//  its own: its first two fields are a `TSLink<T>` (its membership of
//  whatever table owns it), which is what 0x6F7C5090's own destructor
//  unlinks last, inline, treating the object itself as the link.
//
//  Not RTTI-confirmed - nothing in this closure stamps a vtable for it to
//  read a class name off of - so the name is a placeholder pending better
//  evidence, not a claim.  Only the offsets this closure's own functions
//  (Render/facecacheentry.cpp) touch are named; everything else is an exact
//  gap.
//----------------------------------------------------------------------------
struct FACECACHEENTRY
{
    //  0x6F7C4FC0 - not reconstructed this session (Render/
    //  facecacheentry_thunks.cpp): the `eh vector constructor iterator`
    //  rollback and the derived-class vtable restamp over the KERNNODE
    //  table at +0x6C are past this session's budget.
    FACECACHEENTRY();
    //  0x6F7C5090 - not reconstructed this session, same reason plus the
    //  unreproducible SEH/cookie frame (docs/msvc-vc8-idioms.md).
    ~FACECACHEENTRY();
    //  0x6F7C4950 - the rest of the teardown past m_glyphCache's own
    //  release (ReleaseGlyphCache below tail-calls this).  Not
    //  reconstructed this session.
    void ReleaseDetail();
    //  0x6F7C4F90 - Render/facecacheentry.cpp.
    void ReleaseGlyphCache();
    //  0x6F7C55C0 - font-size-in-pixels computation.  Not reconstructed
    //  this session.
    void SetSize(const char* name, int flags, float points);
    //  0x6F7BCFD0 - recompute the entry's own line-height/ascent/descent
    //  metrics.  Not reconstructed this session.
    void RecomputeMetrics();

    void*             m_next;          // +0x00  TSLink<FACECACHEENTRY>::m_next
    int                m_prevlink;      // +0x04  TSLink<FACECACHEENTRY>::m_prevlink
    char               m_reserved08[0x98];
    void*              m_glyphCache;    // +0xA0  released through sub_6F7C7E70
    char               m_reserved0A4[0x120];
    float              m_reserved1C4;   // +0x1C4
};

//----------------------------------------------------------------------------
//  Everything below is a thunk (Render/facecacheentry_thunks.cpp): a
//  minimal class wide enough to spell each redirect's real thiscall
//  signature, with no layout claim beyond that.
//----------------------------------------------------------------------------

//  0x6F7C9210 - the face-cache lookup-or-create FACEDATA itself comes from.
//  A free __fastcall(name, table): two register arguments, no stack ones,
//  so it needs no wrapper class.
void* __fastcall LookupOrCreateFace(const char* name, void* table);

//  0x6F7C8430 - the (name,size) hash bucket walk over the global face table
//  `unk_6FAE71F4`, whose type this batch's dump never resolves.
struct SFaceTableProbeThunk { void* Probe(int size); };

//  0x6F7B9990 - a three-argument TSExplicitList-shaped splice this session
//  did not confidently match against any of storm.h's own Link*() members.
struct SListSpliceThunk { void Splice(void*, void*, unsigned int); };

//  0x6F604FC0 / 0x6F62B910 - a sub-object at CLayoutFrame's own +0x44 this
//  session did not establish a type for, and the event-registry/TLS-slot
//  routine it is handed to.
struct SFrameEventBindingThunk
{
    void SetBinding(void* value);
    void Unregister();
};

#endif
