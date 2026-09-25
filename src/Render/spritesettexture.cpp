//============================================================================
//  0x6F4D32E0 - bind a texture into one of the sprite's model's texture
//  slots.  A no-op when the sprite has no model yet.
//============================================================================
#include "spriteattach.h"

void __fastcall SpriteSetTexture(CSprite* sprite, CTexture* texture, int id)
{
    CModelInstance* model = sprite->m_pModel;
    if (model != 0)
        ReplaceModelTexture(model, id, texture, 1);
}
