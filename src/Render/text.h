//============================================================================
//  The text and font layer - `.\TextBlock.cpp` and the font cache beside it.
//
//  Two shipped names come straight out of the binary:
//
//    ??_7TEXTBLOCK@@6B@                                      (0x6F954A94)
//    ??_7?$TAllocatedHandleObject@VTEXTBLOCK@@$0BAA@@@6B@    (0x6F954A84)
//    ??_7?$TSHashTable@UFONTHASHOBJ@@VHASHKEY_STR@@@@6B@     (0x6F954AC0)
//
//  so a laid-out string is a `TEXTBLOCK`, the pool it comes out of is a
//  `TAllocatedHandleObject<TEXTBLOCK,256>` (the CDataAllocator this repo
//  already reconstructed, at unk_6FAB7D30), and the font cache at
//  0x6FAB7D44 is `TSHashTable<FONTHASHOBJ,HASHKEY_STR>` - the same template
//  storm.h already carries, with the string-key lookup this file's own
//  Containers/tshashstr.inl adds.
//
//  `HTEXTBLOCK` and `HTEXTFONT` are the handle-type tags the two objects
//  pass their allocator and their reference counter; they are diagnostic
//  strings, not names of anything.
//============================================================================
#ifndef TEXT_H
#define TEXT_H

#include "storm.h"
#include "allochashtable.h"    //  HASHKEY_STR and the Storm string ordinals
#include "refcnt.h"
#include "dataallocator.h"

//: The vtable a fresh TEXTBLOCK is stamped with.  Modelled as a literal
//: rather than by declaring TEXTBLOCK's virtuals: nothing in this closure
//: calls one, and declaring them would make this build emit a vtable of its
//: own that the score cannot see and abi_audit reports as WRONG CLASS.  A
//: funcmap.DATA row cannot buy the instruction back either - build_mix.py
//: only binds an address-shaped IDA name, and this one is `??_7TEXTBLOCK@@6B@`.
//: See docs/msvc-vc8-idioms.md, "A vtable stamp is one instruction".
const unsigned int kTextBlockVftable = 0x6F954A94;

//----------------------------------------------------------------------------
//  One laid-out string.  Reference-counted and three words long: nothing in
//  this closure reaches past the engine-side block at +0x08.
//----------------------------------------------------------------------------
struct TEXTBLOCK
{
    //  No constructor: the one site that makes one fills the three fields
    //  itself, in the shipped order +0x04, +0x08, +0x00 - see
    //  Render/textblockbuild.cpp for why it is not placement new.
    void* m_vtable;         // +0x00  ??_7TEXTBLOCK@@6B@
    int   m_refcount;       // +0x04  TRefCnt's
    void* m_engineBlock;    // +0x08  what the layout engine fills in
};

//: unk_6FAB7D30 - TAllocatedHandleObject<TEXTBLOCK,256>, which this repo
//: models as the CDataAllocator its two reached members belong to.
extern CDataAllocator g_textBlockPool;

//----------------------------------------------------------------------------
//  One cached font: a TRefCnt base, then TSHashObject's hash value, bucket
//  link and table-wide link, then the key and the engine-side face.  The
//  TRefCnt base is what puts the bucket link at +0x0C rather than +0x04,
//  which is exactly the `push 0Ch` the table's Initialize makes.
//----------------------------------------------------------------------------
struct FONTHASHOBJ
{
    void*                m_vtable;      // +0x00  TRefCnt's
    int                  m_refcount;    // +0x04
    unsigned int         m_hashval;     // +0x08
    TSLink<FONTHASHOBJ>  m_hashlink;    // +0x0C
    TSLink<FONTHASHOBJ>  m_listlink;    // +0x14
    const char*          m_key;         // +0x1C  "<name>-<flags>-<height>"
    void*                m_face;        // +0x20
};

typedef TSHashTable<FONTHASHOBJ, HASHKEY_STR> CFontTable;

//: dword_6FAB7D44 - the object, not a pointer to it.
extern CFontTable g_fontTable;

//  Storm ordinal 578 - SStrPrintf.
int Storm_578(char* dest, unsigned int size, const char* format, ...);

//  0x6F4C6560 - Math/dividebyconst.cpp: divide by the UI space's Y span.
float __fastcall DivideByConst_6F93C218(float value);

//  --- this module's own functions -------------------------------------------
//  0x6F4CC380 - turn the frame layer's style word into the layout engine's.
int __fastcall RemapTextFlags(int style);

//----------------------------------------------------------------------------
//  A one-dword colour passed by value.  The one call site that takes one
//  reserves the argument slot and stores through a pointer into it rather
//  than pushing the value, which is MSVC materialising a *class* object for a
//  by-value parameter - docs/msvc-vc8-idioms.md, "`push ecx` / `mov eax, esp`
//  / `mov [eax], value` is a class passed by value".  Declared here rather
//  than in Frame/framethunks.h, where it lived while 0x6F4CC470 was still a
//  redirect, so that the tint entry point and its argument type stay
//  together; framethunks.h includes this file.
//----------------------------------------------------------------------------
class CFrameColor
{
public:
    CFrameColor(unsigned int value) : m_value(value) {}
    CFrameColor(const CFrameColor& o) : m_value(o.m_value) {}

    unsigned int m_value;
};

//  0x6F4CC470 - tint a laid-out block.
void __fastcall SetTextBlockTint(void* block, const float* offsets,
                                 CFrameColor color);

//  0x6F4CCD70 - lay a string out into a fresh TEXTBLOCK.  A free
//  `__fastcall`, not a member: the font arrives in ecx *and* the text in
//  edx, which no __thiscall member can spell.  `a`, `b` and `c` keep
//  neutral names because the layout engine's own order for them is not
//  recoverable here - `a` is converted against the UI space's Y span alone
//  and `b`/`c` as an (X, Y) pair, which is not the order the frame's own
//  caller pushes its height and width in.
void* __fastcall BuildTextBlock(void* font, const char* text,
                                const void* color, const float* position,
                                float a, float b, float c,
                                int style, float depth);

//  0x6F4CDE70 - find or make the font for a name, a style and a height.
//  The parameter order is the one the shipped ABI produces: MSVC never
//  register-allocates a float, so `name` takes ecx, `flags` takes edx and
//  `height` is the one stack argument - `retn 4`, as shipped.
void* __fastcall CreateFont(const char* name, float height, int flags);

#endif
