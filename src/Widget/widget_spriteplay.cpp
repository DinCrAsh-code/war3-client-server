//============================================================================
//  0x6F26B860 - CWar3Image::PlaySpriteAnimation.  See widget.h.
//
//  Own translation unit: both of its callers (vtable slots 35 and 36,
//  widget_spriteanim.cpp) reach it with a real `call`, and it in turn tail-
//  calls into the sprite layer for real.
//
//  A widget with no sprite yet answers zero rather than skipping the call
//  and returning whatever was in st0 - the shipped code has an explicit
//  `fldz` on that path, and it is at the *end*, reached by a forward `jz`.
//  Writing the null case as the early return instead inverts the branch and
//  puts the `fldz` in front of the call.
//============================================================================
#include "widget.h"

//  0x6F4D7E50 - start the animation on the sprite and report its length.
//  Out of this session's scope (the model/animation layer: it walks the
//  sprite's own sequence table and its owning model's flags); redirected
//  rather than reconstructed.  __fastcall with the name pointer in edx,
//  matching the shipped `mov edx, [esp+8+arg_0]` right before the call.
__declspec(naked) float __fastcall SpriteStartAnimation(CSprite* sprite,
                                                         const unsigned int* name,
                                                         int flags, int extra)
{
    __asm
    {
        mov     eax, 06F4D7E50h
        jmp     eax
    }
}

float CWar3Image::PlaySpriteAnimation(const unsigned int* name, int flags,
                                    int extra)
{
    CSprite* sprite = m_pSprite;
    if (sprite != 0)
        return SpriteStartAnimation(sprite, name, flags, extra);
    return 0.0f;
}
