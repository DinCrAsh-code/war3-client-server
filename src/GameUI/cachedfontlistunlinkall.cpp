//============================================================================
//  0x6F2FE160 - TSGrowableArray<TSExplicitList<CGameUI::CCachedFont> >::
//  ~TSGrowableArray, the font cache's own bucket array teardown -
//  CGameUI::SFontCacheTable's `m_buckets` (allochashtable.h's own layout:
//  a `TSGrowableArray<TSExplicitList<T> >` at +0x14, so `this` here is
//  really `&table->m_buckets`).
//
//  NOT a plain UnlinkAll (that guess, this file's own first draft, scored
//  0.19 - the real target text resolves the per-bucket call to
//  `TSExplicitList<CGameUI::CCachedFont,-572662307>::UnlinkAll`,
//  GameUI/gameuifontcachelist.cpp's own 0x6F2FAF10, already reconstructed).
//  The generic `TSGrowableArray<T>::~TSGrowableArray()` (Containers/
//  tsarray.inl) walks every bucket calling `T::~T()`, and for
//  `T = TSExplicitList<CCachedFont>` that destructor (Storm/storm.h,
//  already inline: `{ UnlinkAll(); m_terminator.Unlink(); }`) is exactly
//  the call-then-splice-removal shape this address's own dump shows -
//  `m_terminator` is the `TSLink<T>` at +0x04, so the manual
//  `[esi+4]`/`[esi+8]` field reads are `m_terminator.m_next`/`m_prevlink`.
//
//  0x6F2FE230 (`jmp sub_6F2FE160`, cachedfontlistunlinkallalias.cpp) is a
//  one-instruction tail-jmp alias into this same body.
//============================================================================
#include "gameui.h"

//  The tag this destructor's own SMemFree call carries: the *element*
//  type's own RTTI name (agent_worktrees' resolve of aAvTsexplicitli_139,
//  0x6FA75D10), not the array's - the same convention every other
//  TSGrowableArray<T>::~TSGrowableArray() instantiation in this repo
//  follows.
#define TSARRAY_TYPENAME \
    ".?AV?$TSExplicitList@UCCachedFont@CGameUI@@$0?CCCCCCCD@@@"
#include "tsarray.inl"

template TSGrowableArray<TSExplicitList<CGameUI::CCachedFont> >::~TSGrowableArray();
