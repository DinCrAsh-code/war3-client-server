//============================================================================
//  TSFixedArray<BestHeroData> - the snapshot the player record caches at
//  +0x2E4.  0x6F413790 SetAlloc.
//
//  Byte for byte the same body as TSFixedArray<HeroEval>::SetAlloc thirty
//  bytes earlier in the image; the only difference is the allocation tag,
//  ".?AUBestHeroData@@", which is also what names the element type.  Two
//  instantiations are two symbols and there is nothing to fold - this image
//  was linked without /OPT:ICF.
//
//  0x6F418CE0 - SetCount, added this session: CPlayerWar3::Load's own
//  BestHeroData-array reader (playerwar3_loadhelpers.cpp) calls it on the
//  player record's own +0x2E4 field, which turned out to be this exact
//  instantiation (playerwar3.h's own note on how that was traced) rather
//  than a private duplicate - so this file now carries both of this
//  instantiation's out-of-line members, matching the class-level comment
//  above about the one caller of SetCount having been "Player/
//  playerherobest.cpp" before this session's own second caller existed.
//============================================================================
#include "playerheroeval.h"

#define TSFIXEDARRAY_TYPENAME ".?AUBestHeroData@@"
#include "tsfixedarray.inl"

template void TSFixedArray<BestHeroData>::SetAlloc(unsigned int);
template void TSFixedArray<BestHeroData>::SetCount(unsigned int);
