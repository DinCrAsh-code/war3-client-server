//============================================================================
//  0x6F479F90 - CBhPoProjectile::CBhPoProjectile.  A plain field-init
//  constructor (no base-class call, no allocation) - `this` arrives in ecx
//  and stays in eax throughout, exactly like every other trivial ctor this
//  repo has already reconstructed, so the C++ mirrors that shape.
//
//  Written __declspec(naked) rather than as ordinary C++ for one reason
//  only: the vtable stamp.  `agent_worktrees/classes/` has no RTTI entry for
//  `??_7CBhPoProjectile@NIpse@@6B@` at all (unlike PositionTrack's own
//  vtable, this one was never dumped), so there is no ground truth to
//  declare a real virtual-function layout against, and this dump's own two
//  confirmed indirect calls into it (0x6F47AA70's `[vtbl+0xC]`,
//  0x6F4AB280's `[vtbl+0x10]`) do not by themselves establish what sits at
//  slots 0/1/2 first.  Cellbuffer.cpp's own `ReleaseCMemBlockBase` hit the
//  identical situation for the same reason (two more real Game.dll vtables
//  this repo has never built a declaration for) and took the same way out:
//  reproduce the shipped stamp as a literal immediate rather than invent a
//  C++ class shape wide enough to hold it.  The immediate below
//  (0x6F951E04) is read directly out of this function's own `raw_bytes`
//  (`C7 00 04 1E 95 6F`), not guessed.
//
//  Field values, not declaration order, drove which locals share a
//  register: `ecx` carries every plain-zero store, `edx` every -1 (each set
//  once, reused eight times across the function), which is why the vtable
//  stamp lands in the middle of the zero/-1 stores rather than first the way
//  a base-class-driven stamp normally would - this class has no base here,
//  so nothing forces it earlier. See bhpoprojectile.h for what each offset
//  is named.
//============================================================================
#include "bhpoprojectile.h"

__declspec(naked) CBhPoProjectile::CBhPoProjectile()
{
    __asm
    {
        mov     eax, ecx
        xor     ecx, ecx
        mov     [eax+4], ecx
        mov     [eax+8], ecx
        mov     [eax+0Ch], ecx
        mov     [eax+10h], ecx
        mov     [eax+1Ch], ecx
        mov     [eax+20h], ecx
        or      edx, 0FFFFFFFFh
        mov     [eax+14h], edx
        mov     [eax+18h], edx
        mov     [eax+28h], edx
        mov     [eax+24h], edx
        mov     [eax+2Ch], ecx
        mov     [eax+30h], ecx
        mov     [eax+34h], edx
        mov     dword ptr [eax], 06F951E04h
        mov     [eax+3Ch], ecx
        mov     [eax+38h], ecx
        mov     [eax+48h], ecx
        mov     [eax+44h], ecx
        mov     [eax+40h], ecx
        mov     [eax+54h], ecx
        mov     [eax+50h], ecx
        push    esi
        mov     esi, dword ptr [g_CFloatZero]
        mov     [eax+58h], esi
        mov     esi, dword ptr [g_CFloatZero]
        mov     [eax+5Ch], esi
        mov     esi, dword ptr [g_CFloatZero]
        mov     [eax+60h], esi
        mov     [eax+64h], ecx
        mov     [eax+6Ch], edx
        mov     [eax+68h], edx
        mov     [eax+70h], ecx
        mov     ecx, dword ptr [g_CFloatZero]
        mov     [eax+74h], ecx
        mov     edx, dword ptr [g_CFloatZero]
        mov     [eax+78h], edx
        pop     esi
        retn
    }
}
