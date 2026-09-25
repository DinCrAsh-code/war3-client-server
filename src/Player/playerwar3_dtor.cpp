//============================================================================
//  0x6F418A60 - CPlayerWar3::~CPlayerWar3().  See playerwar3.h.
//
//  Own translation unit, same reasoning agentwar3_dtor.cpp/unit_dtorbody.cpp
//  give: slot 1 (playerwar3_deleteself.cpp) keeps its own real `call` into
//  this address rather than having it inlined away.
//
//  A plain member teardown in strict *descending* offset order (the shape
//  a compiler-generated destructor visiting members in reverse declaration
//  order always has), touching (in order): +0x34, +0x2D0 (two raw
//  one-int-argument vtable-slot-0 dispatches on unidentified classes -
//  same shape, no shared name, no dump in this batch resolves either
//  receiver's real type), the +0x2B0/+0x2B4 "dead hero" pointer array
//  (release every live element through TRefCnt::ReleaseSelf and null the
//  slot - CLAUDE.md's own house style for a receiver this session could
//  not name), +0x38 (an unconditional zero-argument call through its own
//  vtable slot 23 *and* a TRefCnt-shaped release, in that order - see
//  ReleaseRef below), +0x310 and +0x24 (RCString - the same destructor
//  Save/Load's own eventual field inventory will confirm both are
//  RCString-typed, called twice from two different offsets), +0x2FC
//  (CAgentTimer), the +0x2EC/+0x2C4 raw Storm-tagged buffer frees
//  ("AUBestHeroData"/"AVIntegerMini" - the same two tags the constructor's
//  own unwind funclets already carry), +0x2AC
//  (TSGrowableArray<CAgentPtr>, agentptrarray.cpp - already reconstructed,
//  reused directly), seven more TRefCnt-shaped releases at
//  +0x294/+0x290/+0x28C/+0x288/+0x284/+0x280/+0x27C (five of which -
//  +0x27C/+0x280/+0x284/+0x290/+0x294 - are the same fields
//  CPlayerWar3::Deactivate already releases through a different helper
//  that also calls slot 2 first; this destructor's own release is the
//  plain TRefCnt-shaped one with no slot-2 notify, and reveals two MORE
//  releasable fields, +0x288/+0x28C, that Deactivate's own five-field
//  pass never reached), the 26-entry AbilityEntry array at +0x40, **+0x38
//  a second time** (a release-only touch with no slot-23 call and no
//  null - see the note on ReleaseField38Twice below), and finally +0x24's
//  own RCString destructor.
//
//  Past this body, the implicit base-class chain (CPlayerWar3 ->
//  CAgentWar3 -> CAgent -> CObserver -> TRefCnt) reproduces the dump's own
//  tail for free: CAgentWar3::~CAgentWar3() (agentwar3_dtor.cpp) is small
//  enough to inline at this call site, and its own `??_7CAgentWar3@@6B@`
//  stamp becomes a dead store immediately overwritten by CAgent::~CAgent()'s
//  own `??_7CAgent@@6B@` stamp (agent_dtor.cpp) - the same "trivial
//  intermediate destructor's own vtable reset gets elided at an inlined
//  call site" shape agentwar3.h's own header comment documents for the
//  CWidget/CSelectable family - so only one extra stamp instruction is
//  visible in the dump, and this file does not need to write it.
//  CAgent::~CAgent()'s own body (the embedded +0x14 CObserver::~CObserver
//  call) and the implicit ~CObserver()/~TRefCnt() base chain after it are
//  already reconstructed (agent_dtor.cpp, observer.cpp, refcnt.h) and
//  need nothing from this file either.
//
//  Same unreproducible-SEH-frame gap as every other CPlayerWar3/CAgent
//  destructor in this family (docs/msvc-vc8-idioms.md), plus the
//  "`eh vector destructor iterator` becomes a plain loop under this
//  build's fixed /EHs-c-" gap for the AbilityEntry array (same file,
//  "An array-member's ... iterator call into a plain loop").
//============================================================================
#include "playerwar3.h"
#include "refcnt.h"
#include "rcstring.h"
#include "agenttimer.h"
#include "storm.h"

typedef void (__thiscall *OneArgMethodFn)(void*, int);
typedef void (__thiscall *ZeroArgMethodFn)(void*);

//  A refcounted slot as the destructor sees it: drop a reference, destroy
//  through vtable slot 0 (TRefCnt::ReleaseSelf) if it was the last, and
//  leave the pointer alone - the same shape unit_dtorbody.cpp's own
//  ReleaseRef already establishes for CUnit::~CUnit().
static void ReleaseRef(void* field)
{
    TRefCnt* p = *(TRefCnt**)field;
    if (p != 0 && --p->m_refcount == 0)
        p->ReleaseSelf();
}

CPlayerWar3::~CPlayerWar3()
{
    void* obj34 = *(void**)((char*)this + 0x34);
    if (obj34)
        ((OneArgMethodFn)(*(void***)obj34)[0])(obj34, 1);

    void* obj2D0 = *(void**)((char*)this + 0x2D0);
    if (obj2D0)
        ((OneArgMethodFn)(*(void***)obj2D0)[0])(obj2D0, 1);

    unsigned int deadHeroCount = *(unsigned int*)((char*)this + 0x2B0);
    void** deadHeroes = *(void***)((char*)this + 0x2B4);
    for (unsigned int i = 0; i < deadHeroCount; ++i)
    {
        void*& slot = deadHeroes[i];
        if (slot)
        {
            ReleaseRef(&slot);
            slot = 0;
        }
    }

    //  +0x38, first touch: an unconditional zero-argument call through
    //  its own vtable slot 23 (+0x5C / 4), then the ordinary TRefCnt
    //  release, then null the field.
    void* obj38 = *(void**)((char*)this + 0x38);
    if (obj38)
        ((ZeroArgMethodFn)(*(void***)obj38)[0x17])(obj38);
    ReleaseRef((char*)this + 0x38);
    *(void**)((char*)this + 0x38) = 0;

    ((RCString*)((char*)this + 0x310))->~RCString();
    ((CAgentTimer*)((char*)this + 0x2FC))->~CAgentTimer();

    void* bestHero = *(void**)((char*)this + 0x2EC);
    if (bestHero)
        SMemFree(bestHero, "AUBestHeroData", -2, 0);

    void* intMini = *(void**)((char*)this + 0x2C4);
    if (intMini)
        SMemFree(intMini, "AVIntegerMini", -2, 0);

    ((TSGrowableArray<CAgentPtr>*)((char*)this + 0x2AC))->~TSGrowableArray();

    ReleaseRef((char*)this + 0x294);
    ReleaseRef((char*)this + 0x290);
    ReleaseRef((char*)this + 0x28C);
    ReleaseRef((char*)this + 0x288);
    ReleaseRef((char*)this + 0x284);
    ReleaseRef((char*)this + 0x280);
    ReleaseRef((char*)this + 0x27C);

    //  The 26-entry AbilityEntry array (playerwar3.h).  The shipped body
    //  reaches this through a genuine `eh vector destructor iterator`
    //  call (protecting against a later element's destructor throwing
    //  mid-teardown); this build's fixed /EHs-c- has no exception model
    //  for that helper to protect, so the compiler emits a plain loop
    //  instead - docs/msvc-vc8-idioms.md, "An array-member's ... iterator
    //  call into a plain loop" - not chased further.
    for (int i = 0; i < 0x1A; ++i)
        m_abilityEntries[i].Destruct();

    //  +0x38, second touch: a release-only re-check with no slot-23 call
    //  and no null-out this time.  By this point in the function the
    //  field is already null (the first touch above always clears it),
    //  so this is dead code at runtime every time - but the compiler
    //  cannot prove that across the AbilityEntry array's own destructor
    //  calls in between, so it stays in the shipped stream and is
    //  reproduced here rather than "cleaned up".
    ReleaseRef((char*)this + 0x38);

    ((RCString*)((char*)this + 0x24))->~RCString();
}
