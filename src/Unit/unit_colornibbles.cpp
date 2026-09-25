//============================================================================
//  0x6F29E8F0 - cunit_agent7 batch (CUnit closure BFS depth 1-2).
//
//  Packs two nibbles into one byte: the low nibble is
//  UnitTypeIntField22C_6F29CA90's own lookup keyed by the unit's type tag
//  at +0x30 (unittypequeries.cpp's own `UnitTypeIntField*` family - see
//  unittypetable.h; +0x22C is a sibling of the already-named +0x26C one
//  this same batch added), the high nibble is the owning player index
//  (slot 59, GetOwningPlayerIndex, unit_thunks.cpp) unless bit 0 of the
//  +0x60 flags dword is set, in which case the high nibble is forced to
//  0xF ("no player" / colourless).
//
//  0x6F29CA90 is not this batch's own worklist address (its lookup target
//  +0x22C is new territory for this file), so it is declared here to the
//  established `UnitTypeIntField<offset>_<addr>` naming convention rather
//  than defined - whichever worklist actually owns it can supply the real
//  body without this TU needing to change.
//============================================================================
#include "unit.h"

extern int __fastcall UnitTypeIntField22C_6F29CA90(unsigned int fourCC);

int CUnit::GetColorPlayerOrGreyNibble()
{
    unsigned int typeFourCC = *(unsigned int*)((char*)this + 0x30);
    int lowNibble = UnitTypeIntField22C_6F29CA90(typeFourCC) & 0xF;

    int highNibble;
    if (m_int60 & 1)
        highNibble = 0xF;
    else
        highNibble = GetOwningPlayerIndex();

    return ((highNibble & 0xF) << 4) | lowNibble;
}

//----------------------------------------------------------------------------
//  Link-completion thunk - 0x6F29CA90 is out of scope for this batch (see
//  the header comment), so nothing else in the tree defines it either. A
//  naked redirect thunk - the same idiom this repo uses for any other
//  out-of-scope callee - makes this TU link without claiming the address
//  as a real reconstruction (no funcmap.py row points at it).
//----------------------------------------------------------------------------
__declspec(naked) int __fastcall UnitTypeIntField22C_6F29CA90(unsigned int)
{
    __asm { mov eax, 06F29CA90h }
    __asm { jmp eax }
}
