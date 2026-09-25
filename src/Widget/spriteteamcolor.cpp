//============================================================================
//  0x6F314CC0 - give the sprite the colour that belongs to `player`.
//
//  Own translation unit: both calls are real.  Two arguments, both in
//  registers and none on the stack (`retn`), which is what a __fastcall
//  free function gives and a thiscall member would not.
//============================================================================
class CSprite;

#include "playercolor.h"

//  0x6F314C80 - push a resolved colour slot at the sprite.  Below the
//  depth-3 cut.  Sprite in ecx, player in edx, slot on the stack.
void __fastcall SetSpriteColorSlot(CSprite* self, unsigned int player,
                                   unsigned int slot);

void __fastcall ApplyTeamColorForPlayer(CSprite* self, unsigned int player)
{
    SetSpriteColorSlot(self, player, ResolvePlayerColorSlot(player));
}
