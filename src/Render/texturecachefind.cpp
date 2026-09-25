//============================================================================
//  0x6F4E1410 - the texture cache's lookup: hash the name, build a key out
//  of the name and the load options, and answer a *new reference* on
//  whatever texture is behind it.
//
//  The key owns a heap copy of the name, so it is a real local with a real
//  destructor: the shipped body duplicates at `.\Texture.cpp` line 0x45 on
//  the way in and frees at line 0x4B on the way out, on both exits.
//============================================================================
#include "texture.h"
#include "refcnt.h"

CTexture* __fastcall TextureCacheFind(const char* path, TEXTUREOPTIONS options)
{
    unsigned int hash = Storm_590(path);
    HASHKEY_TEXTUREFILE key(path, options);

    CTextureHash* node = g_textureTable.Ptr(hash, key);

    CTexture* texture = 0;
    if (node != 0)
        texture = (CTexture*)((TRefCnt*)node->m_texture)->AddRefOrNull();

    return texture;
}
