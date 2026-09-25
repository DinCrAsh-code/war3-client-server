//============================================================================
//  0x6F4C5AD0 - GetStringManager.  Build the one CStringManager on first
//  use, out of Storm's heap and under this module's own file tag.
//
//  The failure arm writes the null back into the global rather than leaving
//  it alone, which is why there are two stores of it in the stream.
//============================================================================
#include "cstringrep.h"

static const char kRCStringCpp[] = ".\\RCString.cpp";

CStringManager* __cdecl GetStringManager()
{
    if (g_pStringManager != 0)
        return g_pStringManager;

    CStringManager* built =
        (CStringManager*)SMemAlloc(0x28, kRCStringCpp, 0x22, 0);

    if (built != 0)
    {
        CStringRepTableConstruct(&built->m_table);
        *(const void**)built = g_vftableCStringManager;
        g_pStringManager = built;
        return built;
    }

    g_pStringManager = 0;
    return 0;
}
