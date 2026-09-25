//============================================================================
//  0x6F443650 SetAlloc and 0x6F443810 SetCount for the save game's own slot
//  array - TSFixedArray over the nine-byte
//  `Net::GAMESETUP_WAR3_0003::SLOT`, which is the RTTI descriptor its
//  allocations tag themselves with.
//============================================================================
#include "savegame.h"
#define TSFIXEDARRAY_TYPENAME ".?AUSLOT@GAMESETUP_WAR3_0003@Net@@"
#include "tsfixedarray.inl"

template void TSFixedArray<Net::GAMESETUP_SLOT>::SetAlloc(unsigned int);
template void TSFixedArray<Net::GAMESETUP_SLOT>::SetCount(unsigned int);
