//============================================================================
//  0x6F4D1EB0 - the indexed value lookup every other CConfigFile accessor goes
//  through.
//
//  Its own translation unit for the same reason src/configfind.cpp is one: both
//  accessors in src/configfile.cpp call it, and with the definition visible
//  MSVC inlines it (measured: GetValue came out 19 instructions instead of 7).
//  The shipped code calls it.
//============================================================================
#include "config.h"

//----------------------------------------------------------------------------
//  0x6F4D1EB0 - the `index`th value of one key, or null.
//
//  `key` comes first because the shipped code takes it in a register and reads
//  `section` back off the stack, so this order is what puts the two arguments
//  in the same places for the FindEntry call.
//
//  The null test is inside the loop, not the loop condition: a chain shorter
//  than `index` gives up, and the same test again after the loop catches an
//  empty chain.
//----------------------------------------------------------------------------
const char* __fastcall CConfigFile::ValueAt(const char* key,
                                            const char* section,
                                            unsigned int index) const
{
    CConfigEntry* entry = FindEntry(section, key);

    for (unsigned int i = 0; i < index; ++i)
    {
        if (entry == 0)
            return 0;
        entry = entry->m_nextValue;
    }

    if (entry != 0)
        return entry->m_value;
    return 0;
}
