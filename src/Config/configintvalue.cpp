//============================================================================
//  0x6F4D1F80 - CConfigFile::GetIntValue: *out = 0, then ValueAt() + parse.
//
//  Its own translation unit for the same reason src/configfind.cpp and
//  src/configvalue.cpp are: it calls ValueAt, and with ValueAt's definition
//  visible in the same TU this build's /O2 inlines it instead of calling it.
//============================================================================
#include "config.h"

//  0x6F4CE050 - declared here rather than in a shared header because nothing
//  else in this file's neighbourhood needs it; see src/parseconfigint.cpp for
//  why the first parameter exists and is unused.
int __fastcall ParseConfigInt(void* unused_ecx, const char* s);

//----------------------------------------------------------------------------
//  Zero the out-parameter first (so a "not found" result still leaves it
//  well-defined) and only overwrite it once ValueAt has actually found
//  something - the shipped code's own store-then-maybe-overwrite shape,
//  matching CConfigFile::GetFloat's already-reconstructed sibling.
//----------------------------------------------------------------------------
int __fastcall CConfigFile::GetIntValue(const char* key, const char* section,
                                        int* out, unsigned int index) const
{
    *out = 0;

    const char* value = ValueAt(key, section, index);
    if (value == 0)
        return 0;

    *out = ParseConfigInt(0, value);
    return 1;
}
