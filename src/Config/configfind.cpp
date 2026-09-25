//============================================================================
//  0x6F4D1E60 - the two-level name lookup at the bottom of the configuration
//  subsystem: find the section, then find the key inside it.
//
//  Its own translation unit, and not part of src/configfile.cpp, only because
//  both functions there call it and it is small enough that MSVC would inline
//  it.  The shipped code calls it, so it has to be a call here too; a
//  translation-unit split is the house way of forcing that (see CLAUDE.md -
//  `__declspec(noinline)` would be a lie about the source, a file boundary is
//  not).
//============================================================================
#include "config.h"

//----------------------------------------------------------------------------
//  Both halves are ordinary lookups, and the second one is a tail call: the
//  shipped code jumps straight into the entry-table Ptr with the key argument
//  it was handed still on the stack, because both take one stack argument and
//  clean it up themselves.  That `jmp` is a real tail call, not a fall-through
//  into the next function.
//----------------------------------------------------------------------------
CConfigEntry* __fastcall CConfigFile::FindEntry(const char* section,
                                                const char* key) const
{
    //  Written "if found" rather than "if not found, return 0" so that the
    //  found case is the fall-through, which is the order the shipped code
    //  has.
    CConfigSection* found = m_sections.Ptr(section);
    if (found != 0)
        return found->m_entries.Ptr(key);
    return 0;
}
