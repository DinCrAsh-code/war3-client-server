//============================================================================
//  0x6F505330 - swap one texture into every layer of the material in this
//  slot whose shared entry names `id`.
//
//  The material is copied first if anything else is holding a reference to
//  it (0x6F502880 is "is the count exactly one?"), which is what makes this
//  a slot rather than a material: the copy has to be written back.  A class
//  with the one pointer in it is also what the ABI says - the slot's address
//  arrives in a register and eight bytes are cleaned, which is `__thiscall`
//  with two stack arguments and nothing else.
//
//  The two arrays are walked in step: the material's own layers at 0x24 each
//  from +0x1C, and the shared half's id entries at 0x2C each from +0x04.
//============================================================================
#include "material.h"
#include "refcnt.h"

void CMaterialRef::ReplaceTexture(int id, CTexture* texture)
{
    CMaterial* material = m_material;
    if (IsField0x4One_6F502880(material) == 0)
    {
        m_material = CloneMaterial(material);
        ((TRefCnt*)material)->Release();
    }

    material = m_material;

    MATERIALTEXTUREIDS* entry = material->m_shared->m_entries;
    CTexLayer* layer = material->m_layers.m_data;

    unsigned int count = material->m_layers.m_count;
    while (count != 0)
    {
        for (int i = 0; i < 2; i++)
        {
            if (entry->m_ids[i] == id)
            {
                CTexture* old = layer->m_textures[i];
                layer->m_textures[i] =
                    (CTexture*)((TRefCnt*)texture)->AddRefOrNull();
                ((TRefCnt*)old)->Release();
                layer->m_alphaMode = GetField0xC_6F4DE1E0(texture);
            }
        }
        entry++;
        layer++;
        count--;
    }
}
