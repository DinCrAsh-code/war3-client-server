//============================================================================
//  0x6F314C80 - SetSpriteColorSlot: push the two team-colour textures that
//  belong to a resolved colour slot onto a sprite.
//
//  Own translation unit: both real calls (ResolvePlayerColorIndex and
//  SpriteSetTexture, twice) leave here for real.
//
//  __fastcall(CSprite* self /*ecx*/, unsigned int player /*edx*/,
//             unsigned int slot /*stack*/), retn 4 - one stack argument.
//  `player` is handed straight through as ResolvePlayerColorIndex's *first*
//  argument and `slot` as its second (the fallback) - the `mov eax,edx` /
//  `mov edx,[esp+arg_0]` / `mov ecx,eax` shuffle at the top is exactly that:
//  edx (player) into ecx, the stack arg (slot) into edx, matching
//  ResolvePlayerColorIndex(unsigned int player, unsigned int fallback)'s own
//  __fastcall(ecx, edx).  The resolved index then indexes two parallel
//  CTexture* tables and each entry is pushed at the sprite through
//  SpriteSetTexture with texture ids 1 and 2, in that order.
//============================================================================
class CSprite;
#include "playercolor.h"

//  0x6F314A70 - GameUI/gameuiplayercolorindex.cpp.
unsigned int __fastcall ResolvePlayerColorIndex(unsigned int player,
                                                unsigned int fallback);

//  0x6F4D32E0 - Render/spritesettexture.cpp.
class CTexture;
void __fastcall SpriteSetTexture(CSprite* sprite, CTexture* texture, int id);

//  dword_6FAB5948 / dword_6FAB5960 - parallel CTexture* tables (one per
//  colour slot) for the sprite's two team-colour texture ids, the way
//  g_pPlayerColorTable (dword_6FAB5954, gameuiplayercolorvalue.cpp) is a
//  parallel table of ARGB values for the UI's own colour lookups.  Both are
//  pointers *to* the table, loaded and then indexed, not the table itself.
extern CTexture* const* g_teamColorSpriteTextures;   // dword_6FAB5948
extern CTexture* const* g_teamGlowSpriteTextures;    // dword_6FAB5960

void __fastcall SetSpriteColorSlot(CSprite* self, unsigned int player,
                                   unsigned int slot)
{
    unsigned int index = ResolvePlayerColorIndex(player, slot);

    SpriteSetTexture(self, g_teamColorSpriteTextures[index], 1);
    SpriteSetTexture(self, g_teamGlowSpriteTextures[index], 2);
}
