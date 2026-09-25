//============================================================================
//  0x6F6540D0 SetAlloc and 0x6F655830 SetCount for the *eight*-byte slot
//  array - `TSFixedArray<Net::GAMESETUP_WAR3_0002::SLOT>`, the middle of the
//  three save-game slot layouts.  SetAlloc's move loop copies two dwords and
//  the array strides by eight; the tag is
//  ".?AUSLOT@GAMESETUP_WAR3_0002@Net@@".
//============================================================================
#include "savegame.h"
#define TSFIXEDARRAY_TYPENAME ".?AUSLOT@GAMESETUP_WAR3_0002@Net@@"
#include "tsfixedarray.inl"

template void TSFixedArray<Net::GAMESETUP_SLOT_0002>::SetAlloc(unsigned int);
template void TSFixedArray<Net::GAMESETUP_SLOT_0002>::SetCount(unsigned int);
