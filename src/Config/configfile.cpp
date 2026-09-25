//============================================================================
//  0x6F4D1EB0, 0x6F4D20C0, 0x6F4D2040, 0x6F4D21B0 - the CConfigFile accessors:
//  one internal indexed value lookup and the three public wrappers over it.
//
//  0x6F4D1E60 (FindEntry) is in src/configfind.cpp so that the calls to it
//  stay calls; see the header comment there.
//============================================================================
#include "config.h"

//----------------------------------------------------------------------------
//  0x6F4D20C0 - the same thing in the argument order the rest of the game
//  uses.
//----------------------------------------------------------------------------
const char* __fastcall CConfigFile::GetValue(const char* section,
                                            const char* key,
                                            unsigned int index) const
{
    return ValueAt(key, section, index);
}

//----------------------------------------------------------------------------
//  0x6F4D2040 - the value parsed as a deterministic float.
//
//  `*out` is cleared before the lookup and again by the caller in every caller
//  in this tree; the shipped code really does write it twice, so the store
//  stays here.  The return value says whether a value was found, not whether
//  it parsed.
//----------------------------------------------------------------------------
int __fastcall CConfigFile::GetFloat(const char* section, const char* key,
                                     CFloat* out, unsigned int index) const
{
    *out = g_CFloatZero;

    const char* text = ValueAt(key, section, index);
    if (text == 0)
        return 0;

    *out = CFloatFromString(text);
    return 1;
}

//----------------------------------------------------------------------------
//  0x6F4D21B0 - does this file carry the key at all.
//----------------------------------------------------------------------------
int __fastcall CConfigFile::HasKey(const char* section, const char* key) const
{
    return FindEntry(section, key) != 0;
}

//----------------------------------------------------------------------------
//  0x6F4D21A0 - does this file carry the named section at all.
//----------------------------------------------------------------------------
int __fastcall CConfigFile::HasSection(const char* name) const
{
    return m_sections.Ptr(name) != 0;
}
