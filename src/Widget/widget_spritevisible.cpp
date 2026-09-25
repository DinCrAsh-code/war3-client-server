//============================================================================
//  0x6F26B820 - CWar3Image::SetSpriteVisible.  See widget.h.
//
//  Own translation unit: CWidget::SetSpriteVisible (vtable slot 37,
//  widget_spriteflag.cpp) reaches it with a real `call` on one path and a
//  second real `call` on the other.
//
//  `force` is an int and not a pointer: every caller passes either a plain
//  flag through (slot 37) or a literal 1 or 0 (slot 61,
//  widget_slot61.cpp), and this function only ever tests it against zero.
//
//  Three paths, two of which share an exit: with `visible` set and a
//  non-zero `force` the "hidden" bit (0x80000000 of m_reserved24) goes on
//  and the sprite is told; with `visible` set and a null owner the bit
//  comes off first; with `visible` clear neither happens and the sprite is
//  told anyway.  The shipped code really does emit two separate
//  `mov ecx,[ecx+28h]` / `call` / `retn 8` tails rather than merging them,
//  so the source has the first one written out as its own early exit.
//============================================================================
#include "widget.h"

//  0x6F4D31D0 - push the flag at the sprite's own model instance.
//  Reconstructed in spritesethidden.cpp; declared here so both call sites
//  below reach it with real calls.  __fastcall: `visible` really does
//  arrive in edx (`test edx,edx` before anything is read off the stack).
void __fastcall SpriteSetHidden(CSprite* sprite, int visible);

void CWar3Image::SetSpriteVisible(int force, int visible)
{
    //  `force` read into its own local up front: the shipped code holds it
    //  in edx across the `visible` test (`cmp [esp+arg_4],0` against a
    //  register `test edx,edx`), and without the local MSVC picks the two
    //  the other way round.
    int target = force;
    if (visible != 0)
    {
        if (target != 0)
        {
            m_reserved24 |= 0x80000000u;
            SpriteSetHidden(m_pSprite, visible);
            return;
        }
        m_reserved24 &= 0x7FFFFFFFu;
    }
    SpriteSetHidden(m_pSprite, visible);
}
