//============================================================================
//  0x6F5053F0 - the shallow walk: every material this model owns, and for
//  each one that names `id`, the swap.
//
//  The shipped build takes the model in `arg_0` and the id in eax; eax is
//  the whole-program convention no MSVC spelling reaches, so the two swap
//  places here - the model is `this` and the id is a stack argument.  The
//  eight bytes the shipped function cleans are preserved exactly, which is
//  the point.
//============================================================================
#include "material.h"

void CModelInstance::ReplaceTexture(int id, CTexture* texture)
{
    for (unsigned int i = 0; i < m_materialCount; i++)
    {
        CMaterial* material = m_materials[i];
        if (MaterialUsesTextureId(material, id))
            ((CMaterialRef*)&m_materials[i])->ReplaceTexture(id, texture);
    }
}
