//============================================================================
//  0x6F3C98F0 / 0x6F3C9980 / 0x6F3C9A10 - the JASS natives `IsVisibleToPlayer`,
//  `IsFoggedToPlayer` and `IsMaskedToPlayer`, all "(RRHplayer;)B".
//
//  Each resolves the player handle, turns the player's own index at +0x30
//  into a one-bit mask, builds a CWorldVec3 out of the two reals plus the
//  simulation's encoded zero, and asks the player table's fog side for that
//  one state (playertablefog.h).  The three differ only in which query they
//  call.
//
//  Three spellings in here are load-bearing and were each measured against
//  the dump:
//
//   * the two reals are taken as `const CFloat*`, not `const int*`.  Both
//     conventions are the same four bytes and every other native in this
//     module takes `const int*`, but here the value goes straight into a
//     CFloat field: `*x` is then already the right type and the vector's
//     three stores each read their source at store time, which is the
//     shipped stream.  Spelled `CFloat(*x)` off an `const int*` the
//     compiler materialises both temporaries up front, spills them into
//     esi/edi and costs six instructions of register save/restore.
//
//   * the vector goes across **by value** and is built directly in the
//     outgoing argument area - `sub esp,0Ch` after the mask has been
//     pushed, then one store per component.  A named local costs a copy;
//     the temporary does not.
//
//   * the player index is copied into an `int` local before the shift.
//     That is the difference between `movzx ecx, byte ptr [eax+30h]` (the
//     shipped instruction, and what lets the mask live in the now-dead
//     eax) and `mov cl, [eax+30h]` with the mask pushed into a callee-saved
//     register instead - six instructions again.  `unsigned char`,
//     `unsigned int`, an inline helper and an explicit `& 31` were all
//     measured and none of them is it.
//
//  Its z is the encoded zero at dword_6FAAE470 (g_CFloatZero) - loaded from
//  the global, not materialised as an immediate, so it is that object and
//  not a `CFloat()`.
//
//  The null-player path returns zero without a `xor eax,eax` for the same
//  reason every other native in this family does: the resolver has already
//  left eax at zero (jassnatives_location.cpp).
//
//  Own translation unit, split from the three `...Loc` natives that call
//  them (jassnatives_locvisibility.cpp) even though the six interleave in
//  one address run: they call each other for real and must not be in a
//  position to inline.
//============================================================================
#include "jassnatives.h"
#include "playertablefog.h"
#include "game.h"

#define g_pPlayerTableFog ((SPlayerTableFogHost*)g_unk6FAB65F4)

int __cdecl JASS_IsVisibleToPlayer(const CFloat* x, const CFloat* y,
                                   int hPlayer)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (!player)
        return 0;

    SPlayerTableSubmitParts* parts = g_pPlayerTableFog->m_pParts;
    int index = player->m_playerIndex;

    return parts->IsPointVisible(CWorldVec3(*x, *y, g_CFloatZero),
                                 1u << index);
}

int __cdecl JASS_IsFoggedToPlayer(const CFloat* x, const CFloat* y,
                                  int hPlayer)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (!player)
        return 0;

    SPlayerTableSubmitParts* parts = g_pPlayerTableFog->m_pParts;
    int index = player->m_playerIndex;

    return parts->IsPointFogged(CWorldVec3(*x, *y, g_CFloatZero),
                                1u << index);
}

int __cdecl JASS_IsMaskedToPlayer(const CFloat* x, const CFloat* y,
                                  int hPlayer)
{
    SJassPlayer* player = ResolvePlayerHandle(hPlayer);

    if (!player)
        return 0;

    SPlayerTableSubmitParts* parts = g_pPlayerTableFog->m_pParts;
    int index = player->m_playerIndex;

    return parts->IsPointMasked(CWorldVec3(*x, *y, g_CFloatZero),
                                1u << index);
}
