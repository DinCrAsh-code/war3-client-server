//============================================================================
//  The three TSHashTable instantiations this target constructs, as the
//  storage each one takes plus the redirect that builds it.  See
//  Containers/hashtablectors.cpp for why all three are redirects and not
//  reconstructions - in one line, the shipped tables carry four vtable slots
//  where this repo's TSHashTable declares two virtuals and defines neither.
//
//  0x28 bytes each: the vtable pointer, the all-nodes TSExplicitList, the
//  probe count, the three words of the bucket array and the mask.
//============================================================================
#ifndef HASHTABLECTORS_H
#define HASHTABLECTORS_H

//  TSHashTable<CGameUI::CCachedFont, HASHKEY_STR> - CGameUI's font cache.
//  Its all-nodes list is TSExplicitList<CGameUI::CCachedFont,-572662307>,
//  whose UnlinkAll (0x6F2FAF10) *is* reconstructed, in
//  GameUI/gameuifontcachelist.cpp; this constructor is its only caller.
struct SFontCacheTable
{
    //  0x6F2FE340 - `retn 0`.  Still a redirect.
    SFontCacheTable* Construct();
    //  0x6F2FE6B0 - `retn 0`.  Self-redirect, same reason Construct() above
    //  is one (hashtablectors.cpp): it stamps `??_7?$TSHashTable@...@@6B@`
    //  at +0x00, and the vtable it would stamp is this repo's own two-slot
    //  one, not the shipped four-slot table.  Body otherwise recovered:
    //  TSHashTable<CCachedFont,HASHKEY_STR>::UnlinkAll(true) (0x6F2FBD80,
    //  out of this batch's own scope - a fifth TSHash-family instantiation,
    //  not reconstructed here), then the bucket array's own
    //  ~TSGrowableArray (0x6F2FE160, GameUI/cachedfontlistunlinkall.cpp)
    //  and the all-nodes list's own UnlinkAll (0x6F2FAF10,
    //  GameUI/gameuifontcachelist.cpp) - the same three-call shape
    //  Containers/commandregtable_dtor.cpp and
    //  Containers/ordertyperegtable_dtor.cpp already establish for their
    //  own instantiations, kept a redirect here for the identical reason.
    void Destruct();
    char m_storage[0x28];
};

//  TSHashTable<CommandReg, HASHKEY_8CC> and TSHashTable<OrderTypeReg,
//  HASHKEY_4CC> - the two registries the command layer keys on an eight- and
//  a four-character code.  Both take link offset 0x0C, i.e. their nodes
//  carry a vtable ahead of the hash link.
struct SCommandRegTable
{
    //  0x6F375850 - `retn 0`.  Still a redirect.
    SCommandRegTable* Construct();
    char m_storage[0x28];
};

struct SOrderTypeRegTable
{
    //  0x6F375B30 - `retn 0`.  Still a redirect.
    SOrderTypeRegTable* Construct();
    char m_storage[0x28];
};

//  TSHashTable<NATIVETOKEN, HASHKEY_STRI> - CTokenizer's name->id map
//  (Storm/tokenizer.h).  Same link offset (0x0C) and storage size (0x28) as
//  the two command-layer tables above, and the same reason for staying a
//  redirect: its mutator, AddToken (0x6F4E9CA0, CTokenizer's only insert
//  path), resolves through a virtual dispatch into AllocNode - the same
//  declared-never-defined virtual hashtablectors.cpp's own note explains -
//  and nothing in any dump here supplies that body, so AddToken is left a
//  thunk too rather than guessed.
struct SNativeTokenTable
{
    //  0x6F4E9440 - `retn 0`.  Still a redirect.
    SNativeTokenTable* Construct();
    char m_storage[0x28];
};

#endif
