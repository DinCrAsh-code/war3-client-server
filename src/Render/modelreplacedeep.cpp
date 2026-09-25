//============================================================================
//  0x6F506A70 - the deep walk: this model's materials, its particle and
//  ribbon emitters, and then - when `recurse` is set - every child in every
//  one of its child lists.
//
//  0x6F506B20 - and the entry point that picks between this and the shallow
//  walk on the model's own flag bit 4.  The shipped body rewrites its own
//  argument slots and tail-jumps into whichever it picked, which is MSVC's
//  tail call and not a source shape.
//============================================================================
#include "material.h"
#include "refcnt.h"

void __fastcall ReplaceTextureDeep(CModelInstance* model, int id,
                                   CTexture* texture, int recurse)
{
    for (unsigned int i = 0; i < model->m_materialCount; i++)
    {
        CMaterial* material = model->m_materials[i];
        if (MaterialUsesTextureId(material, id))
        {
            ((CMaterialRef*)&model->m_materials[i])->ReplaceTexture(id, texture);
            model->ReplaceParticleTexture(id, GetField0xC_6F4DE1E0(texture));
        }
    }

    model->ReplaceRibbonTexture(id, texture);

    if (recurse != 0 && (model->m_flags & 0x10) != 0)
    {
        unsigned int lists = model->m_childListCount;
        if (lists != 0)
        {
            //  Each list is 0x0C bytes and threads its children through
            //  their own +0x04, ending on a non-positive link the way every
            //  TSList in this binary does.
            char* list = (char*)model->m_childLists + 8;
            do
            {
                int link = *(int*)list;
                while (link > 0)
                {
                    ReplaceModelTexture(*(CModelInstance**)(link + 8), id,
                                        texture, 1);
                    link = *(int*)(link + 4);
                }
                list += 0x0C;
                lists--;
            }
            while (lists != 0);
        }
    }
}

void __fastcall ReplaceModelTexture(CModelInstance* model, int id,
                                    CTexture* texture, int recurse)
{
    if ((model->m_flags & 0x10) != 0)
        ReplaceTextureDeep(model, id, texture, recurse);
    else
        model->ReplaceTexture(id, texture);
}
