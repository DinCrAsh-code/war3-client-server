//============================================================================
//  0x6F4E14B0 - the solid-colour cache's lookup.  The colour *is* the hash
//  value and the table's key policy is HASHKEY_NONE, so the walk never
//  compares anything - but the lookup is still handed a key, and what it is
//  handed is the module's one empty HASHKEY_NONE global at unk_6FAB8020.
//
//  **The shipped build takes the colour's address in eax**, which no MSVC
//  convention reaches; it is ecx here and the two call sites pay one move.
//============================================================================
#include "texture.h"
#include "refcnt.h"

CTexture* __fastcall SolidTextureCacheFind(const unsigned int* color)
{
    CSolidTextureHash* node = g_solidTextureTable.Ptr(*color, g_noTextureKey);
    if (node != 0)
        return (CTexture*)((TRefCnt*)node->m_texture)->AddRefOrNull();
    return 0;
}
