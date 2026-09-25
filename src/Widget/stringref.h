//============================================================================
//  The shared string-reference registry: one reference-counted node per
//  distinct string, handed back with a reference already taken.
//
//  The handle-type tag the acquire path passes to AddRef names the object -
//  "HSTRINGREF" - the same way "HCAMERA" and "HTOKENIZER" name theirs.
//
//  The node's layout follows from where its fields sit: TRefCnt's two words
//  first (the vftable and the count), then TSHashObject's own header from
//  +0x08 - the hash there, the bucket link after it, the table-wide link in
//  the eight bytes after that and the key at +0x1C, which is exactly what
//  0x6F4F17B0 reads.
//============================================================================
#ifndef STRINGREF_H
#define STRINGREF_H

#include "refcnt.h"
#include "storm.h"

struct SStringRef : public TRefCnt
{
    unsigned int          m_hashval;            // +0x08
    TSLink<SStringRef>    m_hashlink;           // +0x0C
    char                  m_reserved14[0x08];   // +0x14
    const char*           m_key;                // +0x1C
};

//----------------------------------------------------------------------------
//  The registry.  The object and not a pointer to one, and only the two
//  members this closure calls are declared - both below the dump's depth cut
//  and both redirects in Widget/stringref_thunks.cpp.
//----------------------------------------------------------------------------
struct SStringRefTable
{
    //  0x6F4F09C0 (`retn 4`) - the string-keyed lookup.
    //  Widget/modelhashtablec.cpp.
    SStringRef* Ptr(const char* key);
    //  0x6F4F1680 (`retn 0Ch`) - make a node for this hash.  Still a
    //  redirect (Widget/stringref_thunks.cpp) - dispatches through the
    //  table's own AllocNode virtual, below this dump's depth cut.
    SStringRef* NewNode(unsigned int hashval, void* arg1, void* arg2);

    //  TSHashTable's own generic layout (Storm/storm.h): the bucket array
    //  at +0x1C, the mask at +0x24, established by Ptr's own dump
    //  (`[edi+0x1C]`/`[edi+0x24]`).  Only what Ptr reads is named.
    char                               m_reserved00[0x1C];
    TSExplicitList<SStringRef>*        m_buckets;   // +0x1C
    char                               m_reserved20[4];
    unsigned int                       m_mask;      // +0x24
};

//: unk_6FAB817C.
extern SStringRefTable g_stringRefs;

//  0x6F4F17B0 - Widget/stringrefacquire.cpp.
TRefCnt* __fastcall AcquireStringRef(const char* text);

#endif
