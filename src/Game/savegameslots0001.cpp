//============================================================================
//  0x6F654040 SetAlloc and 0x6F6557E0 SetCount for the *seven*-byte slot
//  array - `TSFixedArray<Net::GAMESETUP_WAR3_0001::SLOT>`, the oldest of the
//  three save-game slot layouts.  Same two bodies as savegameslots.cpp, one
//  element size down: the Storm allocation tag in every call is
//  ".?AUSLOT@GAMESETUP_WAR3_0001@Net@@" and SetAlloc's move loop copies a
//  dword, a word and a byte, which is what pins the element at seven bytes.
//
//  A separate translation unit because it is a separate instantiation with
//  its own two shipped bodies - see docs/msvc-vc8-idioms.md, "Two identical
//  instantiations of a Storm container are two symbols".
//============================================================================
#include "savegame.h"
#define TSFIXEDARRAY_TYPENAME ".?AUSLOT@GAMESETUP_WAR3_0001@Net@@"
#include "tsfixedarray.inl"

template void TSFixedArray<Net::GAMESETUP_SLOT_0001>::SetAlloc(unsigned int);
template void TSFixedArray<Net::GAMESETUP_SLOT_0001>::SetCount(unsigned int);
