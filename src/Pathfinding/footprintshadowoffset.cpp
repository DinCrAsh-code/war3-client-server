//============================================================================
//  CUnit batch-16 (slot-16/107 closure).
//
//  Three more properties on the CFootprintTypeData registry.
//
//  0x6F32D250 - "shadowOffset" x/y (a float pair), 0.0f/0.0f fallback.
//  0x6F32D2A0 - "shadowSize" x/y, the identical shape one property over.
//  0x6F32D390 - "modelColor" (a plain int), -1 fallback (not 0/null the
//  way the pointer properties in footprintshadow.cpp use).
//
//  All three are "return a class by value" free functions: `ecx` on entry
//  is the hidden return buffer, `edx` the type id - the same
//  relationconstant.cpp convention.  `GetFootprintModelColor` compiles
//  1.000 EXACT straight from that C++ shape (footprintpair.h's
//  SFootprintModelColor). The two float-pair getters do not: the shipped
//  zero-fallback arm builds the {0.0f,0.0f} pair through a genuine
//  stack round-trip (`fldz`/`fst`/`mov`/`fstp`/`mov`, both stores routed
//  through GPRs) that no C++ spelling tried reproduces - the compiler
//  always either constructs the pair directly in the hidden buffer via
//  `fstp` (RVO) or copies the found-record arm through `fld`/`fstp`
//  rather than the shipped plain `mov`/`mov`.  Written naked instead, per
//  CLAUDE.md's own rule that a naked body should score 1.000 by
//  construction rather than accept the register/scheduling excuses that
//  apply to compiled code.
//============================================================================
#include "footprinttype.h"
#include "footprintpair.h"

static const char aShadowoffset[] = "shadowOffset";
static const char aShadowsize[]   = "shadowSize";

__declspec(naked) SFootprintFloatPair __fastcall
GetFootprintShadowOffset(unsigned int /*id*/)
{
    __asm
    {
        sub     esp, 8
        mov     eax, edx
        push    esi
        mov     esi, ecx
        mov     edx, offset aShadowoffset
        mov     ecx, eax
        call    GetOrCreateFootprintTypeData
        test    eax, eax
        jnz     short found
        fldz
        fst     [esp+4]
        mov     eax, [esp+4]
        fstp    [esp+8]
        mov     ecx, [esp+8]
        mov     [esi], eax
        mov     [esi+4], ecx
        mov     eax, esi
        pop     esi
        add     esp, 8
        retn
found:
        mov     edx, [eax+80h]
        mov     eax, [eax+84h]
        mov     [esi+4], eax
        mov     [esi], edx
        mov     eax, esi
        pop     esi
        add     esp, 8
        retn
    }
}

__declspec(naked) SFootprintFloatPair __fastcall
GetFootprintShadowSize(unsigned int /*id*/)
{
    __asm
    {
        sub     esp, 8
        mov     eax, edx
        push    esi
        mov     esi, ecx
        mov     edx, offset aShadowsize
        mov     ecx, eax
        call    GetOrCreateFootprintTypeData
        test    eax, eax
        jnz     short found
        fldz
        fst     [esp+4]
        mov     eax, [esp+4]
        fstp    [esp+8]
        mov     ecx, [esp+8]
        mov     [esi], eax
        mov     [esi+4], ecx
        mov     eax, esi
        pop     esi
        add     esp, 8
        retn
found:
        mov     edx, [eax+88h]
        mov     eax, [eax+8Ch]
        mov     [esi+4], eax
        mov     [esi], edx
        mov     eax, esi
        pop     esi
        add     esp, 8
        retn
    }
}

SFootprintModelColor __fastcall GetFootprintModelColor(unsigned int id)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "modelColor");
    if (data == 0)
        return SFootprintModelColor(-1);
    return SFootprintModelColor(data->m_modelColor);
}
