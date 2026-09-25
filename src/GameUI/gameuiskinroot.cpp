//============================================================================
//  0x6F31F4C0 - FindActiveSkinSection.
//
//  The section named by the currently selected skin, or nothing when no skin
//  is selected.  The tail-jump is MSVC's on a `return f(x);`.
//
//  Own translation unit: 0x6F31B4B0 is a real call and 0x6F31F530, the only
//  caller, sits in this same shipped module - it must not be in a position
//  to inline this.
//============================================================================
#include "gameuiskin.h"

SSkinSection* __fastcall FindSkinSection(const char* name);
SSkinSection* __fastcall FindActiveSkinSection();

SSkinSection* __fastcall FindActiveSkinSection()
{
    const char* skin = g_pActiveSkinName;

    if (skin == 0)
        return 0;

    return FindSkinSection(skin);
}
