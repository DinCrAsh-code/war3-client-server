//============================================================================
//  TSFixedArray<HeroEval> - the ranking table 0x6F41B620 builds and throws
//  away.  0x6F413820 SetAlloc, 0x6F419270 SetCount.
//
//  The element type's name is the shipped one, out of the RTTI descriptor
//  string these two hand Storm in the __FILE__ slot: ".?AUHeroEval@@".
//
//  Both in one translation unit for the reason
//  Containers/agentquerypool.cpp gives for its own pair: SetCount's call
//  into SetAlloc stays a real call at this size, which is what the shipped
//  stream shows - and they are in different modules of the image
//  (0x6F413xxx and 0x6F419xxx), which changes nothing here.
//============================================================================
#include "playerheroeval.h"

#define TSFIXEDARRAY_TYPENAME ".?AUHeroEval@@"
#include "tsfixedarray.inl"

template void TSFixedArray<HeroEval>::SetAlloc(unsigned int);
template void TSFixedArray<HeroEval>::SetCount(unsigned int);
