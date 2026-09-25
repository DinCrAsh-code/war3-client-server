//============================================================================
//  CCheatData's own two out-of-line members.  See cheatdata.h for the
//  class and docs/notes/checksum-provider-registry.md for the mechanism.
//============================================================================
#include "cheatdata.h"

//  Storm ordinal 548 - format into the caller's buffer, unbounded (no
//  destination-size argument, unlike Storm_578/SStrPrintf every other file
//  in this repo already declares). Variadic and __cdecl, caller-cleaned -
//  the dump's own `add esp, 0Ch` after three pushed arguments is what
//  settles both.
int __cdecl Storm_548(char* buffer, const char* format, ...);

unsigned int CCheatData::ComputeChecksum()
{
    return m_flags & 0xFFFFEFFF;
}

void CCheatData::DebugDumpTo(void* buffer)
{
    Storm_548((char*)buffer, "Cheat code flags %d\n", m_flags);
}
