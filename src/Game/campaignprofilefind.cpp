//============================================================================
//  0x6F3DF2A0 and 0x6F3DF300 - find a campaign profile by name, and find its
//  index.  Two separate shipped bodies rather than one built on the other:
//  the image was linked without /OPT:ICF, and the two differ only in what
//  they return and in the "not found" value (null against -1).
//
//  Both are members - `retn 4` against a receiver in ecx and one push.  A
//  free `__fastcall` taking the array would take the name in edx and clean
//  four bytes too few.
//============================================================================
#include "campaignprofile.h"

//  Storm_509 - the case-insensitive bounded compare, declared the way
//  every other user of it in this repo does (Config/config.h).
int __stdcall SStrnICmp(const char* a, const char* b, unsigned int max);

CampaignProfile* SCursoredArray::FindByName(const char* name)
{
    if (name == 0)
        return 0;

    for (unsigned int i = 0; i < m_count; i++)
    {
        if (SStrnICmp(name, m_data[i]->Name(), 0x7FFFFFFF) == 0)
            return m_data[i];
    }

    return 0;
}

int SCursoredArray::IndexOfName(const char* name)
{
    if (name == 0)
        return -1;

    for (unsigned int i = 0; i < m_count; i++)
    {
        if (SStrnICmp(name, m_data[i]->Name(), 0x7FFFFFFF) == 0)
            return (int)i;
    }

    return -1;
}
