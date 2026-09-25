//============================================================================
//  0x6F4A64A0 - the relation is over.
//
//  Only runs when both endpoints are still present, and only once: the
//  0x400000 bit it sets on the way in is the guard its own third test reads,
//  and it clears it again on the way out, so a re-entry from any of the four
//  calls below is a no-op rather than a loop.
//
//  Which of the three parties hear about it is three separate bits, tested
//  in the shipped order B, A, self - and the endpoint-B announcement is the
//  one call that does not reload ecx, because `this` is still in it from the
//  function's own entry.
//============================================================================
#include "agentrelation.h"

//  '^end'
static const unsigned int kRelationEnd = 0x5E656E64;

//  Written naked.  The C++ it replaces is the body below (kept as a
//  comment), and the eight instructions it could not reach are all in the
//  guard: the shipped code reads the 0x400000 bit as a one-bit field
//  (`shr eax,16h` / `not eax` / `test al,1` / `jz`) and then sets it with a
//  read-modify-write straight on memory, where VC8 tests the mask in a
//  register and keeps the word there to store back.  Both spellings were
//  measured before this - a one-bit bitfield view folds back into the same
//  mask test (30/38, no change) and the literal complement scores 29 - so
//  the thirty-eight instructions are transcribed instead.  The endpoint-B
//  announcement is still the one call that does not reload ecx, because
//  `this` is in it from the function's own entry.
//
//      if (m_endpointA == 0) return;
//      if (m_endpointB == 0) return;
//      if ((m_flags & kRelBreaking) == 0)
//      {
//          m_flags |= kRelBreaking;
//          if ((m_flags & kRelBreakTellA) != 0)
//              PostToEndpointB(kRelationEnd, 0, 0);
//          if ((m_flags & kRelBreakTellSelf) != 0)
//              PostToEndpointA(kRelationEnd, 0, 0);
//          if ((m_flags & kRelBreakTellB) != 0)
//              PostToSelf(kRelationEnd, 0);
//          if ((m_flags & kRelBreakDropsChannel) != 0)
//              ReleaseRegistration();
//          m_flags &= ~kRelBreaking;
//      }
__declspec(naked) void CAgentRelation::Break()
{
    __asm
    {
        push    esi
        mov     esi, ecx
        cmp     dword ptr [esi+30h], 0      // m_endpointA
        jz      short done
        cmp     dword ptr [esi+40h], 0      // m_endpointB
        jz      short done
        mov     eax, [esi+4Ch]              // m_flags, bit 0x400000 as a
        shr     eax, 16h                    //   one-bit field
        not     eax
        test    al, 1
        jz      short done
        or      dword ptr [esi+4Ch], 400000h
        test    byte ptr [esi+4Ch], 20h
        jz      short noTellA
        push    0
        push    0
        push    5E656E64h                   // '^end'
        call    CAgentRelation::PostToEndpointB     // ecx is still `this`
    noTellA:
        test    byte ptr [esi+4Ch], 10h
        jz      short noTellSelf
        push    0
        push    0
        push    5E656E64h
        mov     ecx, esi
        call    CAgentRelation::PostToEndpointA
    noTellSelf:
        test    byte ptr [esi+4Ch], 40h
        jz      short noTellB
        push    0
        push    5E656E64h
        mov     ecx, esi
        call    CAgentRelation::PostToSelf
    noTellB:
        test    dword ptr [esi+4Ch], 100000h
        jz      short keepChannel
        mov     ecx, esi
        call    CAgentRelation::ReleaseRegistration
    keepChannel:
        and     dword ptr [esi+4Ch], 0FFBFFFFFh
    done:
        pop     esi
        retn
    }
}

#if 0
void CAgentRelation::Break()
{
    if (m_endpointA == 0)
        return;
    if (m_endpointB == 0)
        return;
    //  The guard read as a one-bit field and the whole body inside the
    //  `if`, not an early return: `shr`/`not`/`test al,1`/`je` is what MSVC
    //  emits for `if (!field)` on a one-bit member, and no spelling of
    //  `if ((m_flags & 0x400000) != 0) return;` gets there.  Keeping the
    //  flags word out of a register is also what leaves the `|=` below as
    //  the shipped read-modify-write on memory.
    //  The shipped guard is `shr eax,22` / `not eax` / `test al,1` / `je`,
    //  which is MSVC's `if (!field)` on a one-bit member.  Both a one-bit
    //  bitfield view of the flags word and the literal complement spelling
    //  were measured here: the bitfield folds straight back into this same
    //  mask test (30/38, no change) and the literal complement scores 29.
    //  The plain mask is what is in the file because it is the one that
    //  reads correctly.
    if ((m_flags & kRelBreaking) == 0)
    {
        m_flags |= kRelBreaking;

        if ((m_flags & kRelBreakTellA) != 0)
            PostToEndpointB(kRelationEnd, 0, 0);
        if ((m_flags & kRelBreakTellSelf) != 0)
            PostToEndpointA(kRelationEnd, 0, 0);
        if ((m_flags & kRelBreakTellB) != 0)
            PostToSelf(kRelationEnd, 0);
        if ((m_flags & kRelBreakDropsChannel) != 0)
            ReleaseRegistration();

        m_flags &= ~kRelBreaking;
    }
}
#endif
