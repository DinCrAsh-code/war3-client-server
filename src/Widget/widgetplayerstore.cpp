//============================================================================
//  0x6F26B890 - record the widget's colour slot and pass the player through
//  to its sprite.
//
//  Own translation unit: the sprite-side call is real.
//
//  A thiscall member: both arguments are on the stack and it cleans eight
//  bytes.  `slot` arrives as a full dword and is narrowed on the way in
//  (`movzx edx,al`), so only its low byte reaches +0x24.
//============================================================================
#include "widget.h"

//  0x6F314C80 - push a resolved colour slot at the sprite.  Below the
//  depth-3 cut.
void __fastcall SetSpriteColorSlot(CSprite* self, unsigned int player,
                                   unsigned int slot);

void CWar3Image::StoreOwningPlayer(unsigned int player, unsigned int slot)
{
    //  Two statements, and that is what the shipped code is: a byte store of
    //  zero followed by a *dword* `or`.  A real 8-bit bitfield member
    //  collapses to a single `mov [x],al` here, and the arithmetic spelling
    //  `x = (x & ~0xFF) | v` narrows the mask instead
    //  (`and dword ptr [x],0FFFFFF00h`).  Only the split pair reproduces it,
    //  and the `movzx` that widens `slot` before the `or` comes with it.
    *(unsigned char*)&m_reserved24 = 0;
    m_reserved24 |= (unsigned char)slot;

    if (m_pSprite != 0)
        SetSpriteColorSlot(m_pSprite, player, slot);
}
