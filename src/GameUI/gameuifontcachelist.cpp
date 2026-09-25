//============================================================================
//  0x6F2FAF10 - TSExplicitList<CGameUI::CCachedFont,-572662307>::UnlinkAll,
//  the font cache's all-nodes list.
//
//  One more instantiation of Containers/tslist.inl's walk, byte for byte the
//  same 28 instructions as the four already in this repo (0x6F4BDA40,
//  0x6F4BDAB0, 0x6F4BDB00, 0x6F4CCFD0): the shipped link had no /OPT:ICF, so
//  one template over five element types stays five functions.
//
//  Its one caller is the cache's own constructor (0x6F2FE340), through
//  TSExplicitList::SetLinkOffset.  That constructor is a redirect and stays
//  one - Containers/hashtablectors.cpp carries it and gives the reason,
//  which is *not* the one this comment used to give (storm.h declares
//  AllocNode deliberately non-pure, so the template is not abstract): the
//  shipped table's vtable at 0x6F936190 has four slots where this repo's
//  TSHashTable declares two virtuals and defines neither, so constructing
//  one here would stamp a vtable of this build's own making, of the wrong
//  length, over a shipped class.
//============================================================================
#include "gameui.h"
#include "tslist.inl"

template void TSExplicitList<CGameUI::CCachedFont>::UnlinkAll();
