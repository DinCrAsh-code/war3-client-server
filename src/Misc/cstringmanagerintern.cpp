//============================================================================
//  0x6F4C5C70 - CStringManager::Intern.  Find or create the entry for
//  `text`; a null one answers the shared empty rep without touching the
//  table at all.
//
//  The characters are copied into Storm's heap only when the node that came
//  back is not already pointing at the caller's own buffer - the pointer
//  comparison, not a string one - and whatever was there before is released
//  first.
//============================================================================
#include "cstringrep.h"
#include "rcstring.h"

CStringRep* CStringManager::Intern(const char* text)
{
    if (text == 0)
        return (CStringRep*)&g_emptyRCStringData;

    CStringRep* found = m_table.Ptr(text);
    if (found != 0)
        return found;

    unsigned int hash = Storm_590(text);

    CStringRep* node = m_table.NewNode(hash, 0, 0);
    node->m_hashval = hash;

    if (node->m_key != text)
    {
        if (node->m_key != 0)
            SMemFree((void*)node->m_key, aEDrive1TempBui, 0xA38, 0);

        node->m_key = Storm_507(text, aEDrive1TempBui, 0xA39);
    }

    return node;
}
