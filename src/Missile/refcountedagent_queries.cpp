//============================================================================
//  0x6F284EE0 / 0x6F0774E0 - the two gates
//  CMissile::OnArrival runs against the unit it has just
//  taken a reference to (missile_arrival.cpp).  Both naked redirects there
//  until now.
//
//  Neither calls the other, so they share this translation unit; OnArrival
//  calls both for real, which is why they cannot live in its file.
//============================================================================
#include "missile_thunderbolt.h"
#include "widgetregistersubmit.h"
#include "gamecontext.h"
#include "CFloat.h"

//----------------------------------------------------------------------------
//  0x6F284EE0 - QueryHitAllowed.
//
//  **The same function as CUnit::SubmitToPlayerTable** (unit_playertable.cpp,
//  0x6F285080) with one thing changed: the player index does not come from
//  the caller, it comes from the receiver's own vtable slot 59 (+0xEC), the
//  owning-player accessor.  Everything after that is the identical
//  sequence - the two early outs on the player-slot table, the registry
//  probe through thread-local slot 13 folded into bit 0 of the caller's
//  flags, the submit, and the +0x148 bitfield fallback - so the reasoning
//  is not repeated here; read that file.
//
//  The player index is narrowed to sixteen bits *twice*: `movzx esi, ax`
//  right after the slot call and `movzx esi, si` again just before the
//  push.  Both are in the shipped stream and both survive here because the
//  local is `unsigned short` and the argument is `unsigned int`.
//----------------------------------------------------------------------------
typedef int (__thiscall *OwningPlayerIndexFn)(void* self);

//  misc_trivial_getters.cpp's; the same minimal declaration
//  unit_playertable.cpp makes at its own call site.
struct SBitfield148Owner { int __thiscall TestBit(int bitIndex); };

extern void* g_unk6FAB65F4;

int __thiscall SRefCountedAgent::QueryHitAllowed(void* subject, int flags,
                                                 int extra)
{
    SPlayerTable* table = (SPlayerTable*)g_unk6FAB65F4;
    if (table == 0 || table->m_field3E0 == 0)
        return 0;

    unsigned short player = (unsigned short)
        ((OwningPlayerIndexFn)(*(void***)this)[0xEC / 4])(this);

    void* probe = *((void***)GetThreadLocalSlot(kThreadLocalGameData))[4];
    int extraBit = (probe != 0 && (*(int*)((char*)probe + 4) & 0x200) != 0) ? 1 : 0;

    if (table->SubmitUnit(player, subject, (extraBit != 0) | flags, extra, 0) != 0 ||
        ((SBitfield148Owner*)subject)->TestBit(player) != 0)
        return 1;
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F0774E0 - QueryContextAllows.
//
//  Walk a singly-linked list of candidate objects hanging off the
//  receiver's own +0x1DC handle-ref slot, ask each one to score itself
//  against the context (vtable slot +0x160, a CFloat returned by value),
//  keep the *highest* score, and then ask the winner whether it accepts the
//  context at all (vtable slot +0x16C).  No winner, or a winner that says
//  no, and the answer is no.
//
//  Two shapes are the shipped code's:
//
//  * **the link is a {handle, typeTag} pair, not a pointer.**  Each step is
//    the same "not set is both fields all-ones" test itemhandleresolve.h
//    documents, followed by QueryHandleField0x54 - inlined at both the head
//    and the loop tail, which is why this file has its own helper rather
//    than calling the out-of-line guarded form (0x6F021A00).
//  * **the comparison is strict and the best starts at g_CFloatZero**, so a
//    candidate scoring exactly zero never becomes the winner.  Writing it
//    the other way round (`score > best`) was measured and changes nothing:
//    what differs from the shipped stream is this toolchain's x87 predicate
//    lowering - `test ah, 41h` / `jne` where the shipped build has
//    `test ah, 1` / `jz` - which is the same effect
//    CAgentRegistrySingleton::RebuildDayNightListeners' own BEHAVIOUR entry
//    records, and it is not source-addressable.
//----------------------------------------------------------------------------
typedef CFloat (__thiscall *ScoreContextFn)(void* self, void* context);
typedef int    (__thiscall *AcceptContextFn)(void* self, void* context);

struct SContextCandidate
{
    char               m_reserved00[0x24];
    SOptionalHandleRef m_next;      // +0x24 / +0x28
};

//  The head-and-tail resolve, inlined at both use sites by MSVC.
static void* ResolveIfSet(SOptionalHandleRef* ref)
{
    if ((ref->m_handle & (unsigned int)ref->m_typeTag) == 0xFFFFFFFFu)
        return 0;
    return QueryHandleField0x54(ref);
}

int __thiscall SRefCountedAgent::QueryContextAllows(void* context)
{
    CFloat best = g_CFloatZero;
    SContextCandidate* winner = 0;

    SContextCandidate* node = (SContextCandidate*)
        ResolveIfSet((SOptionalHandleRef*)((char*)this + 0x1DC));

    while (node != 0)
    {
        CFloat score = ((ScoreContextFn)(*(void***)node)[0x160 / 4])(node, context);
        if (*(const float*)&best.m_bits < *(const float*)&score.m_bits)
        {
            best = score;
            winner = node;
        }
        node = (SContextCandidate*)ResolveIfSet(&node->m_next);
    }

    if (winner == 0)
        return 0;
    if (((AcceptContextFn)(*(void***)winner)[0x16C / 4])(winner, context) == 0)
        return 0;
    return 1;
}
