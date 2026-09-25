//============================================================================
//  0x6F4E3AE0 - put one texture into the cache under a name and a set of
//  load options.
//
//  The node's key is assigned rather than constructed, which is what the two
//  Storm calls at `.\Texture.cpp` lines 0x50 and 0x51 are: the old copy is
//  freed and a new one taken.  The local key is built at line 0x45 and freed
//  at 0x4B like TextureCacheFind's.
//============================================================================
#include "texture.h"
#include "refcnt.h"

void __fastcall TextureCacheAdd(const char* path, TEXTUREOPTIONS options,
                                CTexture* texture)
{
    unsigned int hash = Storm_590(path);
    HASHKEY_TEXTUREFILE key(path, options);

    CTextureHash* node = g_textureTable.NewNode(hash, 0, 0);
    node->m_hashval = hash;
    node->m_key = key;
    node->m_texture = (CTexture*)((TRefCnt*)texture)->AddRefOrNull();
}
