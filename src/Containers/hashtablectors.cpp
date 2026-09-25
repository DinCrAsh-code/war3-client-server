//============================================================================
//  0x6F2FE340, 0x6F375850, 0x6F375B30 and 0x6F4E9440 - four TSHashTable
//  constructors, redirected rather than reconstructed.
//
//  All four are one body over a different instantiation:
//
//      0x6F2FE340  TSHashTable<CGameUI::CCachedFont, HASHKEY_STR>
//      0x6F375850  TSHashTable<CommandReg,           HASHKEY_8CC>
//      0x6F375B30  TSHashTable<OrderTypeReg,         HASHKEY_4CC>
//      0x6F4E9440  TSHashTable<NATIVETOKEN,          HASHKEY_STRI>
//
//  - the instantiation names are the shipped ones, off the
//  `??_7?$TSHashTable@...@@6B@` each one stamps at +0x00.  Past that stamp
//  the body is Storm/storm.h's own layout being initialised: the all-nodes
//  list's link offset poisoned to 0xDDDDDDDD and its terminator self-linked,
//  m_probes and all three words of the bucket array cleared, the link offset
//  then set for real (0x14 for the font cache's node, 0x0C for the other
//  two) through TSExplicitList::SetLinkOffset - which is the `cmp` /
//  UnlinkAll / re-Reset sequence in the middle - and m_mask left at -1.
//
//  **Why these are redirects and not reconstructions.**  Constructing one
//  needs MSVC to stamp a vtable, and the vtable it would stamp is not the
//  shipped one.  `ida_query vtable` on all three shipped tables
//  (0x6F936190, 0x6F940844, 0x6F940858) reports **four** slots each; the
//  TSHashTable this repo models declares two virtuals, and both of them -
//  the destructor and AllocNode - are declared and never defined, because
//  nothing in any dump here is either of them.  So making the template
//  concrete would (a) fail to link until two virtual bodies were invented,
//  and (b) install a two-slot vtable of this build's own making over a
//  four-slot shipped class, which is a wrong-length vtable that no
//  instruction score can see and that dispatches the wrong function for
//  every index at or past the mistake - exactly what tools/check_vtables.py
//  exists to stop.  A redirect leaves the shipped constructor, and
//  therefore the shipped vtable, in place.  0x6F4E9440's own body is
//  instruction-for-instruction the same shape as the other three (same
//  poisoned link offset, same terminator self-link, same final link-offset
//  fixup, this one to 0x0C) and its mutator, AddToken (0x6F4E9CA0), resolves
//  through the same missing-body virtual dispatch the comment above
//  describes - nobody has queried its shipped vtable's own slot count, but
//  nothing in any dump here supplies AllocNode's body regardless of what
//  that count turns out to be, so it is grouped with the other three rather
//  than guessed at.
//
//  Spelled as `Construct` members on a struct per table rather than as C++
//  constructors, for the reason GameUI/selectmodector.cpp already records:
//  MSVC will not take __declspec(naked) on a constructor.  Each hands `this`
//  back, which is what the shipped `mov eax, esi` before the `retn` does.
//============================================================================

#include "hashtablectors.h"

#define HASHTABLE_CTOR_THUNK(addr)   \
    {                                \
        __asm { mov eax, addr }      \
        __asm { jmp eax }            \
    }

__declspec(naked) SFontCacheTable* SFontCacheTable::Construct()
    HASHTABLE_CTOR_THUNK(06F2FE340h)

__declspec(naked) SCommandRegTable* SCommandRegTable::Construct()
    HASHTABLE_CTOR_THUNK(06F375850h)

__declspec(naked) SOrderTypeRegTable* SOrderTypeRegTable::Construct()
    HASHTABLE_CTOR_THUNK(06F375B30h)

__declspec(naked) SNativeTokenTable* SNativeTokenTable::Construct()
    HASHTABLE_CTOR_THUNK(06F4E9440h)

#define HASHTABLE_DTOR_THUNK(addr)   \
    {                                \
        __asm { mov eax, addr }      \
        __asm { jmp eax }            \
    }

//  0x6F2FE6B0 - see the declaration in hashtablectors.h.  Same reasoning as
//  Construct() above and as Containers/commandregtable_dtor.cpp /
//  Containers/ordertyperegtable_dtor.cpp's own destructor thunks: a real
//  reconstruction would stamp this repo's own two-slot TSHashTable vtable
//  over the shipped four-slot one.
__declspec(naked) void SFontCacheTable::Destruct()
    HASHTABLE_DTOR_THUNK(06F2FE6B0h)
