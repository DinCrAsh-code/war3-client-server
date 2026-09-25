//============================================================================
//  0x6F4E3BB0 - and the insert.  The colour's address arrives in eax in the
//  shipped build, as it does in the lookup beside it.
//============================================================================
#include "texture.h"
#include "refcnt.h"

void __fastcall SolidTextureCacheAdd(const unsigned int* color,
                                     CTEXTUREREF texture)
{
    unsigned int hash = *color;

    CSolidTextureHash* node = g_solidTextureTable.NewNode(hash, 0, 0);
    node->m_hashval = hash;
    node->m_texture =
        (CTexture*)((TRefCnt*)texture.m_texture)->AddRefOrNull();
}
