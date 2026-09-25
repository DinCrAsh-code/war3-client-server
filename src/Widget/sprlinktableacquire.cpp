//============================================================================
//  0x6F4F00D0 / 0x6F4F0170 - the same acquire-or-build over the two
//  registries ".\SprLinkTable.cpp" builds, at that module's lines 0x6B and
//  0x86.
//
//  The two are one shipped module, two objects, two registries and two
//  constructors; nothing else about them differs, and the only thing this
//  call tree establishes about *which* is which is the caller -
//  SetSpriteModel (0x6F4D8D60) reaches the first for its second model
//  argument and the second for its third, and hands the two results to two
//  different virtual setters on the sprite (slots +0x5C and +0x60).  So the
//  second one is named for being the second and nothing more is claimed.
//
//  Same unreproducible __CxxFrameHandler3 frame as
//  Widget/spranimlistacquire.cpp, which see.
//
//  Own translation unit: six real calls out of it, and the two functions
//  never call each other.
//============================================================================
#include "sprresource.h"
#include "storm.h"

SSprLinkTable* __fastcall AcquireSprLinkTable(void* key, void* model,
                                              const char* path)
{
    SSprLinkTable* found = g_sprLinkTables.Find(key);

    if (found == 0)
    {
        void* raw = SMemAlloc(0x2C, ".\\SprLinkTable.cpp", 0x6B, 0);

        SSprLinkTable* made = 0;
        if (raw != 0)
        {
            made = (SSprLinkTable*)raw;
            made->Construct(model, path);
        }

        g_sprLinkTables.Insert(made, key);
        found = made;
    }

    found->m_refs++;
    return found;
}

//----------------------------------------------------------------------------
//  0x6F4F0170 - the module's second table, line 0x86.
//----------------------------------------------------------------------------
SSprLinkTableSecondary* __fastcall AcquireSprLinkTableSecondary(
                                              void* key, void* model,
                                              const char* path)
{
    SSprLinkTableSecondary* found = g_sprLinkTablesSecondary.Find(key);

    if (found == 0)
    {
        void* raw = SMemAlloc(0x2C, ".\\SprLinkTable.cpp", 0x86, 0);

        SSprLinkTableSecondary* made = 0;
        if (raw != 0)
        {
            made = (SSprLinkTableSecondary*)raw;
            made->Construct(model, path);
        }

        g_sprLinkTablesSecondary.Insert(made, key);
        found = made;
    }

    found->m_refs++;
    return found;
}
