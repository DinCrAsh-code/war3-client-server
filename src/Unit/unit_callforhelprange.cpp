//============================================================================
//  CUnit batch-15 (slot 72 closure).  0x6F27BCA0 - the unit's own "call for
//  help" search range: a creep-owned unit (GetOwningPlayerIndex() >= 0xC,
//  slot 59) reads "Misc/CreepCallForHelp"; a real player's unit with
//  m_flags5C bit 4 set reads a fixed constant (dword_6FAAE5B0); anything
//  else reads "Misc/CallForHelp".
//
//  Not a normal callable: this is a genuine "tail-chunk with a custom
//  calling convention" - its one real call site (CUnit::Method_0x24's own
//  body, 0x6F2A40D0, out of this batch's scope) has `this` already live in
//  edi and points esi at a caller-owned stack CFloat before the call, and
//  the body itself never sets either register up - no MSVC __declspec
//  reaches "this in edi, hidden-return pointer in esi", so this stays a
//  literal naked reproduction of the exact register usage rather than a
//  typed CUnit member.  `retn` bare (no cleanup - both "arguments" are
//  registers, not stack words).
//============================================================================
#include "unit.h"
#include "gamecontext.h"

static const char s_creepCallForHelp[] = "CreepCallForHelp";
static const char s_callForHelp[]      = "CallForHelp";
static const char s_misc[]             = "Misc";

//  dword_6FAAE5B0 - see funcmap.py's DATA table.
extern const CFloat g_callForHelpFixedRange;

__declspec(naked) void GetCallForHelpRange_6F27BCA0()
{
    __asm
    {
        mov     eax, [edi]
        mov     edx, [eax+0ECh]
        mov     ecx, edi
        call    edx
        cmp     eax, 0Ch
        jl      short notCreep
        push    0
        push    offset s_creepCallForHelp
        mov     edx, offset s_misc
        mov     ecx, esi
        call    GetConfigFloat
        mov     eax, esi
        retn
    notCreep:
        test    byte ptr [edi+5Ch], 4
        jz      short notFixed
        mov     eax, dword ptr g_callForHelpFixedRange
        mov     [esi], eax
        mov     eax, esi
        retn
    notFixed:
        push    0
        push    offset s_callForHelp
        mov     edx, offset s_misc
        mov     ecx, esi
        call    GetConfigFloat
        mov     eax, esi
        retn
    }
}
