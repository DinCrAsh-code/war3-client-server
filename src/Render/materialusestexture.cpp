//============================================================================
//  0x6F502840 - does any of this material's layers name that texture id?
//
//  The walk is over the *shared* half's entries, two ids each, and the id
//  arrives in edi in the shipped build - the whole-program register
//  convention no MSVC spelling reaches - so it is the second parameter here
//  and the two call sites pay a move.
//============================================================================
#include "material.h"

int __fastcall MaterialUsesTextureId(const CMaterial* material, int id)
{
    CMaterialShared* shared = material->m_shared;

    unsigned int count = shared->m_count;
    MATERIALTEXTUREIDS* entry = shared->m_entries;

    while (count != 0)
    {
        for (int i = 0; i < 2; i++)
        {
            if (entry->m_ids[i] == id)
                return 1;
        }
        entry++;
        count--;
    }

    return 0;
}
