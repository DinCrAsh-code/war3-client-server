//============================================================================
//  The order-path callees that sit below JASS_IssueTargetOrderById's own
//  dump depth cut: redirects into the shipped bodies so that the functions
//  above them link and the real code runs.
//
//  NOT reconstructions.  Delete each one as its real body lands.
//
//  Every argument count here was read off the shipped function's own
//  trailing bytes with `ida_query func_at` + `ida_query bytes`, not
//  inferred from the call site, because that is the one thing about a thunk
//  nothing else in this pipeline can check (tools/thunk_abi_audit.py needs
//  a dump heading and none of these has one):
//
//      0x6F279A90  C2 04 00  retn 4    CUnit::FindAbilityForOrder
//      0x6F2A49C0  (tail jmp 0x6F29ED60, itself `add esp,1Ch` / C3)
//                            retn 0    CUnit::StopMovement
//      0x6F26DD30  C3        retn 0    CUnit::RestartPropertyRamp
//      0x6F29DFF0  C2 08 00  retn 8    CUnit::BeginOrder
//      0x6F4728D0  C2 04 00  retn 4    CAgent::SetTrackedTarget
//
//  What each one is, and why it is named that:
//
//  * **CUnit::FindAbilityForOrder** (461 instructions) walks the ability
//    list at +0x1DC and returns the one that should carry `order` out.  It
//    switches on the order's own agile type id - '+ord', 'ord.', 'or.2',
//    'ordt', 'ort2' and two more - and asks a different checker per shape,
//    keeping the candidate whose slot 0x248 answers lowest.  Twelve of its
//    own callees are another level down again, which is why it is here.
//  * **CUnit::StopMovement** re-publishes the movement object at +0x164,
//    flushes the pending ramp when the +0x54 counter is zero, and clears
//    bits 20 and 21 of the flag word at +0x5C - the bit PrepareForNewOrder
//    tests before calling it.
//  * **CUnit::RestartPropertyRamp** re-arms a controller slot on the object
//    at +0x28 through BindControllerSlotRamp (0x6F4E8720,
//    GameUI/cameracontrollerbind.h), picking between two callbacks by
//    comparing the floats at +0x2F8 and +0x2FC, or - when +0x1EC is zero -
//    hands the value at +0x300 straight over instead.
//  * **CUnit::BeginOrder** (101 instructions, and the one entry here that
//    *does* have a dump body) hands the order to whichever of three
//    starters matches its class - 0x6F299A00 for a plain '+ord',
//    0x6F299C40 for an 'ord.' point order, 0x6F299E90 for an 'ordt' target
//    order whose target still resolves and 0x6F299C40 again when it does
//    not - and then announces it.  Its own six callees are a level below
//    this dump and two of them have no name yet, which is why it is a
//    redirect and not a body.
//  * **CAgent::SetTrackedTarget** - see agent.h.
//
//  Own translation unit for the reason unitorderthunks.cpp gives for its
//  own: a redirect whose callee is visible gets inlined and there is no
//  `jmp` left.
//============================================================================
#include "unit.h"
#include "agent.h"

#define ADDR_THUNK_PATH(sig, addr)                \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

ADDR_THUNK_PATH(void* CUnit::FindAbilityForOrder(void*), 0x6F279A90)
//  CUnit::StopMovement (0x6F2A49C0) was here.  Its dump arrived as part of
//  JASS_IssueTargetOrderById's own closure and it is a real reconstruction
//  now - unitorderstopmovement.cpp.
ADDR_THUNK_PATH(void CUnit::RestartPropertyRamp(), 0x6F26DD30)
ADDR_THUNK_PATH(void CUnit::BeginOrder(void*, int), 0x6F29DFF0)
//  CAgent::SetTrackedTarget (0x6F4728D0) was here.  Its dump arrived and it
//  is a real reconstruction now - Agent/agentsettrackedtarget.cpp.

#undef ADDR_THUNK_PATH
