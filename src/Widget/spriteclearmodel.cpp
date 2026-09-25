//============================================================================
//  0x6F4D2C90 - drop everything a sprite holds on behalf of a loaded model:
//  the animation list at +0x30, whatever vtable slots 0x5C and 0x60 own, the
//  counted reference at +0x20, the three flag bytes at +0x34..+0x36 and the
//  second counted reference at +0x60.
//
//  A `__fastcall` free function and not a member, for the reason
//  sprite.h's other entry points are: the whole 0x6F4D3xxx family is
//  spelled that way here so that one shipped address keeps one C++ name
//  across the several call trees that reach it.
//
//  The two indirect calls are hand-written slot dispatches.  Their argument
//  count is not a guess - both shipped sites push exactly one dword (`push
//  ebx`, with ebx zero) and the caller never adjusts esp afterwards, so each
//  slot is a `__thiscall (void*, int)` cleaning four bytes.  CSprite has no
//  committed vtable in docs/targets/vtables, so vtable_dispatch_audit.py
//  cannot judge either one; this note is the evidence in its place.
//
//  Own translation unit: all three calls are real.
//============================================================================
#include "sprite.h"
#include "sprresource.h"
#include "refcnt.h"

void __fastcall ClearSpriteModel(CSprite* self)
{
    if (self->m_pModelData != 0)
        ReleaseSprAnimList((SSprAnimList*)self->m_pModelData);
    self->m_pModelData = 0;

    typedef void (__thiscall *ClearSlotFn)(CSprite*, int);
    //  slot 0x5C, then slot 0x60, each with a zero.
    ((ClearSlotFn)(*(void***)self)[0x5C / 4])(self, 0);
    ((ClearSlotFn)(*(void***)self)[0x60 / 4])(self, 0);

    if (self->m_pModel != 0)
    {
        ((TRefCnt*)self->m_pModel)->Release();
        self->m_pModel = 0;
    }

    self->m_flag36 = 0;
    self->m_flag35 = 0;
    self->m_flag34 = 0;

    if (self->m_pRef60 != 0)
    {
        ((TRefCnt*)self->m_pRef60)->Release();
        self->m_pRef60 = 0;
    }
}
