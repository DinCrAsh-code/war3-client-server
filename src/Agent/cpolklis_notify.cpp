//============================================================================
//  0x6F4AA250 - NIpse::CPoLkLis::NotifyEndpointAIfTagged, vtable slot 8.
//
//  The REAL override this class (and the rest of the "*Lis"/"*Mod"
//  listener/modifier subset of the 9-class Po-modifier/listener family) has
//  at slot 8 - the plain "*Flag"/"*Link"/"*Real" subset instead inherits the
//  shared no-op there. See cpolklis.h's own file header for the family-wide
//  split.
//
//  `arg` is filtered before anything happens: null, or a mismatch on either
//  of its own +0x00/+0x08 dwords against two fixed magic values, and the
//  function is a no-op. Once past the filter it builds the same 11-dword
//  event context every vtable-slot-0x20 receiver in this codebase takes
//  (Agent/agenttickrelay.h's own SAgentTickEventCtx,
//  Agent/agentrelationevents.cpp's own three senders) - here with a fixed
//  "^lis" tag (0x5E6C6973) at field0 rather than the usual sender-identity
//  "^rel", a second fixed constant (0x606C6B6C) duplicated into BOTH
//  field4 and the eventCode slot, `this` as the subject, and the usual
//  {0,0,0,0,0} run for field10-field20 and {-1,-1} for field24/field28
//  matching Agent/agentrelationevents.cpp's own FillContext shape exactly.
//  Then, only if `this`'s own +0x30 (CAgentRelation::m_endpointA,
//  Agent/agentrelation.h) is non-null, fires it through that endpoint's own
//  vtable slot 0x20 (0x20 bytes = index 8), the same dispatch
//  PostToEndpointA/PostToEndpointB/PostToSelf already use one level up the
//  chain.
//
//  Written naked, matching Agent/agentrelationevents.cpp's own precedent
//  for this exact family of functions: the shipped build interleaves the
//  filter branch, the 11-dword context build and the endpoint re-read/
//  dispatch in a specific register-and-store order that a straightforward
//  C++ rewrite does not reliably reproduce (that file's own header explains
//  why for its three siblings), and a naked transcription is unambiguous
//  and exact by construction (CLAUDE.md's own rule for `__declspec(naked)`
//  bodies - every register and instruction here is one this comment names).
//  Neither magic constant is claimed to be readable ASCII text beyond the
//  "^lis" one (0x5E6C6973, matching the '^xxx' four-char-code convention
//  Agent/agentrelation.h's own file header documents); the other three -
//  the two filter values compared against `arg` and the 0x606C6B6C payload
//  constant - are left as plain magic numbers, not guessed at.
//============================================================================
#include "cpolklis.h"

namespace NIpse {

__declspec(naked) void* CPoLkLis::NotifyEndpointAIfTagged(const void* /*arg*/)
{
    __asm
    {
        mov     edx, [esp+4]
        xor     eax, eax
        sub     esp, 2Ch
        cmp     edx, eax
        jz      done
        cmp     dword ptr [edx+8], 606C6B63h
        jnz     done
        cmp     dword ptr [edx], 5E70726Fh
        jnz     done
        mov     edx, 606C6B6Ch
        mov     [esp+4], edx                // field4
        mov     [esp+8], edx                // eventCode
        or      edx, 0FFFFFFFFh
        cmp     [ecx+30h], eax              // m_endpointA
        mov     dword ptr [esp], 5E6C6973h  // field0: '^lis'
        mov     [esp+0Ch], ecx              // subject: this
        mov     [esp+10h], eax              // field10
        mov     [esp+14h], eax              // field14
        mov     [esp+18h], eax              // field18
        mov     [esp+1Ch], eax              // field1C
        mov     [esp+20h], eax              // field20
        mov     [esp+28h], edx              // field28 = -1
        mov     [esp+24h], edx              // field24 = -1
        jz      done
        mov     ecx, [ecx+30h]
        mov     eax, [ecx]
        mov     eax, [eax+20h]
        lea     edx, [esp]
        push    edx
        call    eax
    done:
        add     esp, 2Ch
        retn    4
    }
}

}  // namespace NIpse
