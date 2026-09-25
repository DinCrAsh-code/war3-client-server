//============================================================================
//  0x6F74CBB0 - the one SStormSingletonA method stormsingleton_terrain.cpp
//  dispatches to that is still a redirect.  It walks two different tint
//  tables (+0x9DC as dwords, +0x19DC as words) depending on a mode flag at
//  +0x21E4 and reaches two more Storm-internal helpers no dump here
//  carries; see its declaration in stormsingletona.h.
//
//  Its former file-mate StoreTint (0x6F7424B0) is now reconstructed in
//  stormsingletonstoretint.cpp.
//
//  Own translation unit purely so the naked body does not sit in the same
//  file as the caller that must reach it with a real `call`.
//============================================================================
#include "stormsingletona.h"

__declspec(naked) void SStormSingletonA::QueryTintAt(unsigned int*, const FVec3*)
{
    __asm
    {
        mov     eax, 06F74CBB0h
        jmp     eax
    }
}
