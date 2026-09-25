//============================================================================
//  0x6F476F40 / 0x6F47BAF0 - see widgetpathrefadjust.cpp.
//
//  Both are thiscall members, not __fastcall free functions: each takes its
//  `add` flag on the *stack* and cleans it (`retn 4`), where a __fastcall
//  free function would take it in edx and clean nothing.  That is not a
//  cosmetic difference - a `ret` where the shipped code has a `ret 4`
//  unbalances the caller's stack on the first call, which is the FATAL
//  bucket tools/abi_audit.py exists to catch.
//
//  SPathRefHandle is layout-identical to game.h's CPathRef and is the same
//  object.  It is spelled again here rather than adding a method to
//  CPathRef because game.h is included by nearly every translation unit in
//  the repo, and re-touching it shifts the compiler's $Tnnnn temporary
//  names through every already-matching listing (see funcmap.py's
//  CSyncChannel::Extrapolate note for what that costs).
//============================================================================
#ifndef WIDGETPATHREFADJUST_H
#define WIDGETPATHREFADJUST_H

#include "CFloat.h"

//  The path data hanging off a resolved handle object's own +0x98, and the
//  only field either function touches.
struct SPathRefCounted
{
    char m_reserved00[0x40];
    int  m_pinCount;      // +0x40
};

//  Same shape as CPathRef (game.h): handle at +0x08, type tag at +0x0C.
struct SPathRefHandle
{
    //  0x6F476F40.
    void AdjustPathRefCount(int add);

    //  0x6F474860 / 0x6F4748C0 / 0x6F4748F0 - pathrefflags.cpp.  All three
    //  thiscall members: the flag is pushed and cleaned (`retn 4`).
    void SetPathFlagBlocking(int on);
    void SetPathFlagWalkable(int on);
    void SetPathFlagVisible(int on);

    char         m_reserved00[0x08];
    unsigned int m_handle;      // +0x08
    int          m_typeTag;     // +0x0C
};

//  A growable array of them: count at +0x04, data at +0x08.
struct SPathRefArray
{
    //  0x6F47BAF0.
    void AdjustPathRefArray(int add);

    //  0x6F47BCD0 - see pathref_save.cpp.  It is what named the two CFloats
    //  at +0x10/+0x14: they are written out right after the array itself,
    //  through the same CFloat writer CWidget::Save uses for its own
    //  +0x40.  It also reads m_count as a *word* (`movzx ebp, word ptr
    //  [edi+4]`), which the `(unsigned short)` cast below reproduces -
    //  AdjustPathRefArray reads the whole dword, so the field stays one.
    void Save(class CDataStoreScratch* store);

    //  0x6F47C2B0 - Save's counterpart.  See pathref_load.cpp.
    void Load(class CDataStore* store);

    //  0x6F47C1A0 - see pathrefarrayresize.cpp.  `handleOwner` is a
    //  CWidget*, not `this` - its handle/type pair at +0x0C/+0x10 is only
    //  used to validate and pass an owner pointer through to each newly
    //  allocated element, the same +0x0C/+0x10 pair CWidget::
    //  AdjustWidgetPathRefs (widgetpathref.cpp) reads off its own `this`.
    void ResizeAndFillMasks(class CWidget* handleOwner, unsigned int newCount,
                            const unsigned int* masks);

    char              m_reserved00[0x04];
    unsigned int      m_count;     // +0x04
    SPathRefCounted** m_pItems;    // +0x08
    char              m_reserved0C[0x04];
    CFloat            m_x;         // +0x10
    CFloat            m_y;         // +0x14
};

#endif
