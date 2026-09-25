//============================================================================
//  0x6F2833E0 / 0x6F26FE60 / 0x6F26FE70 - three small CUnit members the unit
//  factory (0x6F29F990, jasscreateunit.h) calls in a row as it finishes a
//  new unit:
//
//      6F29FC35    mov     ecx, [esp+200h+var_1EC]   ; the new unit
//      6F29FC39    call    sub_6F2833E0              ; ... is it eligible?
//      6F29FC3E    test    eax, eax
//      6F29FC40    jz      short loc_6F29FC49
//      6F29FC42    push    0
//      6F29FC44    call    sub_6F26FE60              ; ecx still the unit
//
//  which is what types all three: the same receiver the factory has just
//  built, and CUnit's own m_flags5C / m_int248 are the fields the first one
//  reads (unit.h already names both).
//
//  One translation unit for two modules: none of the three calls another,
//  so there is nothing for the compiler to inline across.
//============================================================================
#include "unit.h"

//  0x6F332F00 / 0x6F333C10 - the two handlers the pair forwards to,
//  __fastcall with the unit in ecx and the caller's flag in edx.  Both take
//  the unit in the register it is already in, which is why neither forwarder
//  has a `mov ecx` of its own.  Not reconstructed.
void __fastcall UnitCreationNotifyA_6F332F00(CUnit* unit, int flag);
void __fastcall UnitCreationNotifyB_6F333C10(CUnit* unit, int flag);

//----------------------------------------------------------------------------
//  0x6F2833E0 - the eligibility test above.  Bit 4 of m_int248 must be set
//  and none of three bits of m_flags5C may be.
//
//  `test byte ptr [ecx+248h], 10h` rather than a dword test is what the
//  mask itself produces: MSVC narrows a test whose mask fits in one byte,
//  so `m_int248 & 0x10` is already the shipped instruction and spelling it
//  as a byte member would be a layout claim this tree does not support.
//----------------------------------------------------------------------------
int CUnit::IsEligibleForCreationNotify()
{
    //  One conjunction, not two early returns - the same shape
    //  JASS_UnitAddItemById's own note describes: the shipped code falls
    //  through both tests onto a single `mov eax, 1` and sends both
    //  refusals forward to one shared `xor eax, eax` tail.  Written as two
    //  `return 0;` guards MSVC hoists the zero into the middle and turns
    //  the second test into a `neg`/`sbb`/`add` bool instead of a branch.
    if ((m_int248 & 0x10) != 0 && (m_flags5C & 0xC0000100) == 0)
        return 1;

    return 0;
}

//----------------------------------------------------------------------------
//  0x6F26FE60 / 0x6F26FE70 - two one-line forwarders.  `call` and `retn 4`
//  rather than a tail `jmp`, because the callee takes both arguments in
//  registers and cleans nothing, so these two have to clean their own
//  stack argument themselves.
//----------------------------------------------------------------------------
void CUnit::NotifyCreatedA(int flag)
{
    UnitCreationNotifyA_6F332F00(this, flag);
}

void CUnit::NotifyCreatedB(int flag)
{
    UnitCreationNotifyB_6F333C10(this, flag);
}
