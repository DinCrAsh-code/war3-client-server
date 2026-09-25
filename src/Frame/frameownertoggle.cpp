//============================================================================
//  CFrame's own slots 6/7/8's (+0x18/+0x1C/+0x20) real handoff bodies -
//  0x6F5FAF80/0x6F5FAFC0/0x6F5FB000.  Split into their own translation unit
//  from the three vtable slots that call them (frameownertoggleslots.cpp):
//  with both in the same TU, `/Ob2` inlined a full copy of each into *both*
//  of its own slot's two call sites even though neither is `inline` -
//  docs/msvc-vc8-idioms.md's "A local helper called from two branches..."
//  entry - and only moving the definition out of sight forces the real
//  `call` the shipped body has on both branches.
//
//  Each toggles which one CLayer-descended object currently holds a token
//  kept in its own global (g_frameTokenOwner66C/674/678): if `candidate`
//  is not already the holder, notify the object losing it through its own
//  vtable slot 0x74/0x94/0xA0, store the new holder, and notify the object
//  gaining it through slot 0x70/0x90/0x9C - both dispatched on the
//  *object*, not on `this`.  Slot 6's own handoff also resets the shared
//  elapsed-time float (flt_6FACE660 = g_layerElapsedTime, clayer.h).
//============================================================================
#include "frame.h"

extern CLayer* g_frameTokenOwner66C;   // dword_6FACE66C
extern CLayer* g_frameTokenOwner674;   // dword_6FACE674
extern CLayer* g_frameTokenOwner678;   // dword_6FACE678

namespace {
typedef void (__thiscall *NotifyFn)(void*);
}  // namespace

//  0x6F5FAF80.  `__fastcall`: the shipped call sites leave `candidate` in
//  ecx and never push it - Method_0x18's own copy into eax ahead of the
//  call (frameownertoggleslots.cpp) is priming its *own* return value
//  (the same candidate, unclobbered by the call only in eax) rather than
//  an argument, which is what says this returns the candidate rather than
//  being `void`.
CLayer* __fastcall SetOwner66C(CLayer* candidate)
{
    if (candidate == g_frameTokenOwner66C)
        return candidate;

    if (g_frameTokenOwner66C != 0)
    {
        NotifyFn lost = (NotifyFn)(*(void***)g_frameTokenOwner66C)[0x74 / 4];
        lost(g_frameTokenOwner66C);
    }

    g_frameTokenOwner66C = candidate;

    if (candidate != 0)
    {
        NotifyFn gained = (NotifyFn)(*(void***)candidate)[0x70 / 4];
        gained(candidate);
    }

    g_layerElapsedTime = 0.0f;
    return candidate;
}

//  0x6F5FAFC0.
CLayer* __fastcall SetOwner674(CLayer* candidate)
{
    if (candidate == g_frameTokenOwner674)
        return candidate;

    if (g_frameTokenOwner674 != 0)
    {
        NotifyFn lost = (NotifyFn)(*(void***)g_frameTokenOwner674)[0x94 / 4];
        lost(g_frameTokenOwner674);
    }

    g_frameTokenOwner674 = candidate;

    if (candidate != 0)
    {
        NotifyFn gained = (NotifyFn)(*(void***)candidate)[0x90 / 4];
        gained(candidate);
    }
    return candidate;
}

//  0x6F5FB000.
CLayer* __fastcall SetOwner678(CLayer* candidate)
{
    if (candidate == g_frameTokenOwner678)
        return candidate;

    if (g_frameTokenOwner678 != 0)
    {
        NotifyFn lost = (NotifyFn)(*(void***)g_frameTokenOwner678)[0xA0 / 4];
        lost(g_frameTokenOwner678);
    }

    g_frameTokenOwner678 = candidate;

    if (candidate != 0)
    {
        NotifyFn gained = (NotifyFn)(*(void***)candidate)[0x9C / 4];
        gained(candidate);
    }
    return candidate;
}
