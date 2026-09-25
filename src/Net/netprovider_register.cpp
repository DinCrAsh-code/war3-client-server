//============================================================================
//  NetProviderBase_Register, 0x6F672A60 (`Net::NetProvider::Method_25`'s
//  own call site - netprovider_method0x90.cpp - only had this declared as
//  a naked thunk in src/Misc/unreconstructed_thunks.cpp; this session
//  pulled its own dump (`ida_query calltree_asm 0x6F672A60 --depth 3`) and
//  the body turned out small and self-contained).
//
//  A lazy, critical-section-guarded, refcounted "register once" gate: the
//  first caller (the global refcount transitioning 0->1) does the real
//  work - one call through `self`'s own vtable slot 10 (+0x28), then, only
//  if either argument is non-zero, a second call through slot 15 (+0x3C)
//  with the real event callback `sub_6F672720` as that call's own fourth
//  argument - and every later caller, while the refcount stays above zero,
//  skips both vtable calls and just reuses the cached word result the
//  first caller left in `word_6FACFE80`.
//
//  Why this function matters beyond its own body: `sub_6F672720` is the
//  callback this install actually registers, and its own xrefs
//  (`ida_query xrefs 0x6F672340`) show it calling straight into
//  `Probably_W3GS_ActionHandler` (0x6F672340, this same dump's root) - the
//  same "outer event callback wraps an inner packet-type dispatch" shape
//  `ContainsW3GSBytePacketCheck` already has for `NetProviderLTCP`'s own
//  listener thread. So this function, not the dispatcher's own root
//  address, is where dispatcher1's install chain actually starts, and it
//  sits in the 0x6F672xxx address neighbourhood that also carries three
//  ".\\NetRouter.cpp" debug-assert strings a few hundred bytes further on
//  (sub_6F672AF0's own body, and its callees sub_6F672B7A/CE0/6F673034) -
//  see docs/targets/Probably_W3GS_ActionHandler.md's "Client vs server,
//  corrected" section for the full caller-chain trace this is part of,
//  and how it contrasts with the sibling dispatcher's own NetClient.cpp
//  install chain.
//
//  `sub_6F672720` itself (0x6F672720..0x6F672A60, immediately before this
//  function's own start - the two are laid out back to back) is not
//  reconstructed; taking its address is all this function needs, the same
//  `(void*)0x6F...` idiom src/Net/netproviderbnet_lifecycle.cpp and
//  src/Net/netproviderbnet_timer.cpp already use for an unreconstructed
//  callback target.
//
//  dword_6FACFE88 (the refcount), dword_6FACFE84 (the cached slot-10
//  result, itself never read back by this function - purely a diagnostic/
//  bookkeeping cache for something else in this call tree that was never
//  reached) and word_6FACFE80 (the cached slot-15 result, which IS read
//  back by every later caller) are plain shipped globals with no
//  cross-referenced type beyond how this one function uses them;
//  stru_6FACFEE0 is the CRITICAL_SECTION the whole gate is guarded by.
//  All four are `funcmap.DATA` rows (netprovider_register.cpp's own
//  comment there) rather than defined here - real, shared shipped storage,
//  not a private copy (see CLAUDE.md's own `link_check.py` DATA-row
//  warning for why that distinction matters live).
//============================================================================
#include <windows.h>

void __fastcall EnterCritSec(CRITICAL_SECTION* cs);
void __fastcall LeaveCritSec(CRITICAL_SECTION* cs);

extern CRITICAL_SECTION g_netProviderRegisterLock;    // stru_6FACFEE0
extern int              g_netProviderRegisterRefCount;      // dword_6FACFE88
extern int              g_netProviderRegisterSlot10Cache;   // dword_6FACFE84
extern short            g_netProviderRegisterSlot15Cache;   // word_6FACFE80

//  0x6F672720 - the real event callback registered through slot 15 below;
//  not reconstructed. Its address is all this function needs.
#define W3GSActionRouterCallback ((void*)0x6F672720)

int __fastcall NetProviderBase_Register(void* self, int edxArg, int stackArg)
{
    EnterCritSec(&g_netProviderRegisterLock);

    int prevCount = g_netProviderRegisterRefCount;
    g_netProviderRegisterRefCount = prevCount + 1;

    short result;
    if (prevCount != 0)
    {
        //  Already registered by an earlier call still holding the
        //  refcount up - reuse the cached result rather than touching the
        //  vtable again.
        result = g_netProviderRegisterSlot15Cache;
    }
    else
    {
        typedef int (__thiscall *Slot10Fn)(void*);
        Slot10Fn slot10 = (Slot10Fn)(*(void***)self)[0x28 / 4];
        g_netProviderRegisterSlot10Cache = slot10(self);

        if ((short)edxArg != 0 || (short)stackArg != 0)
        {
            typedef short (__thiscall *Slot15Fn)(void*, int, int, void*);
            Slot15Fn slot15 = (Slot15Fn)(*(void***)self)[0x3C / 4];
            short slot15Result =
                slot15(self, edxArg, stackArg, W3GSActionRouterCallback);
            g_netProviderRegisterSlot15Cache = slot15Result;
            if (slot15Result != 0)
            {
                //  Registration failed - undo the refcount bump and the
                //  slot-10 cache so the next caller retries for real.
                g_netProviderRegisterRefCount -= 1;
                g_netProviderRegisterSlot10Cache = 0;
            }
            result = slot15Result;
        }
        else
        {
            //  Neither argument asked for the real slot-15 registration -
            //  report whatever word_6FACFE80 already held (uninitialised
            //  shipped state on a genuine first call with both arguments
            //  zero; this is a direct transcription of that, not a bug).
            result = g_netProviderRegisterSlot15Cache;
        }
    }

    LeaveCritSec(&g_netProviderRegisterLock);
    return result;
}
