//============================================================================
//  .\RCString.cpp - Storm's interned, reference-counted strings.
//
//  The file name is the shipped one (CStringManager's own allocation tags
//  itself ".\\RCString.cpp"), and so is every class name here: the table's
//  constructor stamps `??_7?$TSHashTable@VCStringRep@@VHASHKEY_STR@@@@6B@`
//  and the singleton's stamps `??_7CStringManager@@6B@`.
//
//  CStringRep has FONTHASHOBJ's shape exactly - a TRefCnt base, then
//  TSHashObject's hash value, bucket link and table-wide link, then the
//  characters - which is what puts the bucket link at +0x0C (the `0Ch` the
//  table's Initialize writes into every bucket's link offset) and the
//  table-wide link at +0x14 (the `14h` its constructor writes into the
//  all-nodes list).
//============================================================================
#ifndef CSTRINGREP_H
#define CSTRINGREP_H

#include "storm.h"
#include "allochashtable.h"     // HASHKEY_STR, Storm_508/590/501/503

//----------------------------------------------------------------------------
//  One interned string.
//----------------------------------------------------------------------------
struct CStringRep
{
    void*                m_vtable;      // +0x00  TRefCnt's
    int                  m_refcount;    // +0x04
    unsigned int         m_hashval;     // +0x08
    TSLink<CStringRep>   m_hashlink;    // +0x0C
    TSLink<CStringRep>   m_listlink;    // +0x14
    const char*          m_key;         // +0x1C  the characters themselves
};

typedef TSHashTable<CStringRep, HASHKEY_STR> CStringRepTable;

//----------------------------------------------------------------------------
//  The process-wide table.  Only the two members this call tree reaches are
//  named; everything else is TSHashTable's.
//----------------------------------------------------------------------------
struct CStringManager
{
    //  0x6F4C5C70 - retn 4.  Find or create the entry for `text`, copying
    //  the characters into Storm's own heap the first time.  A null `text`
    //  answers the shared empty rep without touching the table.
    CStringRep* Intern(const char* text);

    CStringRepTable m_table;
};

//: dword_6FAB7D14 - the one CStringManager, or null before the first use.
extern CStringManager* g_pStringManager;

//: 0x6F4C5AD0 - build it on that first use.
CStringManager* __cdecl GetStringManager();

//: 0x6F4C5290 - the table's own constructor, written as a plain function
//: because a real C++ one would make this build emit a vtable for
//: TSHashTable and with it an unresolved AllocNode (see storm.h).
CStringRepTable* __fastcall CStringRepTableConstruct(CStringRepTable* table);

//: The two vtables the shipped code stamps - ??_7CStringManager@@6B@ and
//: ??_7?$TSHashTable@VCStringRep@@VHASHKEY_STR@@@@6B@.  This build emits
//: neither: nothing here is constructed as C++ (the table's constructor is a
//: plain function, see above), so there is no class for MSVC to lay a table
//: out for.  They are stored as *local* symbols rather than declared and put
//: in funcmap.DATA, because a DATA row whose IDA name is not address-shaped
//: has nothing for the injection binder to resolve and would leave the .mix
//: with an unresolved external.  The instruction compare cannot tell the
//: difference - both sides canonicalise to `mov [reg], SYM` - but the
//: *behaviour* can, so this is on the target write-up's arm-with-caution
//: list: an object built by this build's own constructor carries a private,
//: all-zero table, and nothing in this closure dispatches through either.
extern void* g_vftableCStringManager[1];
extern void* g_vftableCStringRepTable[1];

//: aEDrive1TempBui - Storm's own __FILE__ for the hash module, which IDA
//: prints truncated ("e:\\Drive1\\temp\\buildwar3x\\Storm\\H"...).
//: Referenced as a symbol rather than retyped as a literal: a literal that
//: was not byte-for-byte the shipped one would put a second string in the
//: image, and the compare cannot see the difference either way.
extern "C" const char aEDrive1TempBui[];

//: Storm ordinal 507 - SStrDupA, the heap copy Intern keeps.
char* __stdcall Storm_507(const char* text, const char* logfilename,
                          int logline);

#endif
