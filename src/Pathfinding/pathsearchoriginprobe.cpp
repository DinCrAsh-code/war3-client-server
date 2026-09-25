//============================================================================
//  0x6F468E40 and its own trampoline 0x6F468F90.
//
//  Both take a world-space (x, y) pair (register-passed - ecx/edx on the
//  outer function, forwarded unchanged from the trampoline's own ecx/edx),
//  flush each axis's distance from the map origin the exact way
//  Sync/timesyncbounds.cpp's own `SHandleWithType::FlushedOriginDelta`
//  already does (`((delta - 0x3000000) ^ delta) >> 31` as the flush mask,
//  `delta + 0xFD800000` as the scaled value - the same bit-trick, not
//  reduced to a shared helper because neither of these two sessions' own
//  call trees pin down what the trick is really testing any better than
//  the first one did), then probe `CTimeSync::GetPathSearch()`'s search
//  object with the flushed pair through `sub_6F4A3790` - a `CPathSearch`
//  member this pass does not otherwise reach a dump for, so it is declared
//  locally (see below) rather than given a real name.
//
//  **`sub_6F468E40` reads a fourth argument that arrives in `edi`, set up
//  by its own caller and never loaded from any of its own three declared
//  stack slots.**  No MSVC calling convention spells "the fourth parameter
//  arrives in edi" - the same class of whole-program-optimised argument
//  `docs/notes/hook-abi-crash-classes.md`'s own `ServerTock_0x27_PacketHandler`
//  already documents - so this reconstruction declares it as an ordinary
//  fourth parameter instead. That is fine here specifically because both
//  ends of the one real call site (0x6F468F90's own trampoline) are
//  reconstructed together in this same file and agree with each other; it
//  would not be safe to call this declaration's shape into the *shipped*
//  0x6F468E40 the way a thunk does.
//
//  A genuine `__except_handler4`-shaped SEH frame (cookie-XORed,
//  EBP-omitted, scope-table trampoline appended after `retn`) wraps
//  0x6F468E40's whole body - docs/msvc-vc8-idioms.md's own unreproducible
//  shape - so it scores low by construction; the call sequence, field
//  offsets (`CGameData::m_originX`/`m_originY`, `CPathSearch::m_searching`)
//  and flush arithmetic past the frame match the dump.
//
//  `CPathSearch::m_searching`'s own header comment (pathfind.h) says it is
//  "set for the duration of one FindFreePoint, and restored by its
//  __finally" - this function sets and restores the same flag around a
//  *different* call (`sub_6F4A3790`, not `FindFreePoint` itself, whose own
//  real address and signature - 0x6F4A3FF0 - do not match this call site's
//  two-stack-argument shape), so it is a second, independent caller of the
//  same re-entrancy convention rather than FindFreePoint's own internals.
//============================================================================
#include "game.h"
#include "pathfind.h"
#include "widgetpathrefadjust.h"

//  0x6F4A3790 - a `CPathSearch` member this pass has no dump for; declared
//  locally with the shape this one call site proves (`this` in ecx, two
//  stack pointers, `retn 8`) rather than guessed at more broadly.
struct SPathSearchOriginProbeTarget
{
    int TestFlushedOrigin(const CGridVec2* flushed, const void* tag);
};
__declspec(naked) int SPathSearchOriginProbeTarget::TestFlushedOrigin(const CGridVec2*, const void*)
{
    __asm { mov eax, 0x6F4A3790 }
    __asm { jmp eax }
}

//  The flush idiom itself, transliterated exactly as
//  Sync/timesyncbounds.cpp's own FlushedOriginDelta has it - see that
//  file's header for why it stays a literal bit-trick rather than a named
//  helper.
static CFloat FlushOriginDelta(const CFloat& value, const CFloat& origin)
{
    CFloat delta = value - origin;
    int bits = delta.m_bits;
    int flush = ((bits - 0x3000000) ^ bits) >> 31;
    return CFloat(~flush & (bits + 0xFD800000));
}

//----------------------------------------------------------------------------
//  0x6F468E40.  `retn 0Ch` plus the implicit `edi` fourth argument - see
//  this file's own header comment.
//----------------------------------------------------------------------------
int ProbePathSearchNearOrigin(const CFloat* x, const CFloat* y,
                                     const void* tag, SPathRefHandle* pathRef)
{
    CPathSearch* search = g_pTimeSync->GetPathSearch();

    CFloat flushedY = FlushOriginDelta(*y, g_pGameData->m_originY);
    CFloat flushedX = FlushOriginDelta(*x, g_pGameData->m_originX);

    if (pathRef != 0)
        pathRef->AdjustPathRefCount(1);

    int savedSearching = search->m_searching;
    search->m_searching = 1;

    int result = 1;
    CGridVec2 flushed(flushedX, flushedY);
    if (((SPathSearchOriginProbeTarget*)search)->TestFlushedOrigin(&flushed, tag) != 0)
        result = 0;

    if (pathRef != 0)
        pathRef->AdjustPathRefCount(0);

    search->m_searching = savedSearching;
    return result;
}

//----------------------------------------------------------------------------
//  0x6F468F90 - the trampoline: pack a byte tag into the top byte of a
//  scratch dword (the low three bytes are always zero, from the shipped
//  `movzx`/`shl 18h`) and forward the register-passed (x, y) pair and the
//  path-ref handle straight through.  `retn 8`.
//----------------------------------------------------------------------------
int ProbePathSearchNearOriginPacked(const CFloat* x, const CFloat* y,
                                           unsigned char tag, SPathRefHandle* pathRef)
{
    unsigned int packedTag = (unsigned int)tag << 24;
    return ProbePathSearchNearOrigin(x, y, &packedTag, pathRef);
}
