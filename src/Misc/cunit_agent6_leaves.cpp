//============================================================================
//  A handful of unrelated leaves and near-leaves reached from CUnit's own
//  vtable closure (BFS depth 1-2), grouped the way misc_leaves_batch5.cpp
//  already groups its own unrelated small functions - see that file and its
//  matching dump (asm/processed/misc_leaves_batch5_calltree_asm.md) for the
//  convention this batch follows.
//============================================================================
#include "agent.h"

//----------------------------------------------------------------------------
//  0x6F0D90A0 / 0x6F1D6180 / 0x6F2717C0 - three bare "return this literal
//  FourCC" leaves (`mov eax,<imm32>; retn`), the same shape as every
//  required-type-id getter this codebase already has a name for
//  (misc_rawcode_getters.cpp).  Each constant read MSB-first: 0x414F6275 is
//  'A','O','b','u' - "AObu"; 0x41437370 is "ACsp"; 0x7461736B is "task".
//  None decodes to a member of the known agile-type-id family this call
//  tree's own callers reach (CUnit's own '+w3u', the ability-module 'A'-
//  prefixed tags), so - matching checkedslot_batch25.cpp's own rule for a
//  constant with no established meaning - each keeps an address-derived
//  name rather than a guessed one.
//----------------------------------------------------------------------------
unsigned int GetFourCC_6F0D90A0()
{
    return 0x414F6275;
}

unsigned int GetFourCC_6F1D6180()
{
    return 0x41437370;
}

unsigned int GetFourCC_6F2717C0()
{
    return 0x7461736B;
}

//----------------------------------------------------------------------------
//  0x6F034D10 - drop a reference held at +0x20 through its own vtable slot 0
//  and mark the slot empty (-1, not null - the shipped store is a literal
//  0FFFFFFFFh).  0x6F333480 is below this agent-6 slice's own BFS cut and
//  not itself a worklist entry for any of the eight concurrent slices, so it
//  stays thunked here.
//----------------------------------------------------------------------------
//  Not in an anonymous namespace: a parameter type namespaced that way
//  demangles with a compiler-generated suffix
//  (`?A0x<hash>@@`) that tools/verify.py's funcmap lookup does not parse
//  back to this function's plain name, so the entry never scores at all.
struct SReleasableAt0x20_6F034D10
{
    void ReleaseSelf_6F333480();   // 0x6F333480 - thunked
};

__declspec(naked) void SReleasableAt0x20_6F034D10::ReleaseSelf_6F333480()
{
    __asm { mov eax, 0x6F333480 }
    __asm { jmp eax }
}

struct SField20Host_6F034D10
{
    char                     m_reserved00[0x20];
    SReleasableAt0x20_6F034D10* m_field20;   // +0x20
};

void __fastcall ReleaseAndClearField20(SField20Host_6F034D10* self)
{
    self->m_field20->ReleaseSelf_6F333480();
    self->m_field20 = (SReleasableAt0x20_6F034D10*)0xFFFFFFFF;
}

//----------------------------------------------------------------------------
//  0x6F075560 - tail-call: fetch this object's own observer handle
//  (0x6F1D2F20) and hand it, with `this`, to the handle's own unhook
//  (0x6F1DB690).  Both callees are below this agent-6 slice's own BFS cut
//  and not claimed by any of the eight concurrent slices, so both stay
//  thunked here.
//----------------------------------------------------------------------------
namespace {

extern "C" __declspec(naked) void* __fastcall GetObserverHandle_6F1D2F20(void*)
{
    __asm { mov eax, 0x6F1D2F20 }
    __asm { jmp eax }
}

extern "C" __declspec(naked) void __fastcall
UnhookObserverHandle_6F1DB690(void*, void*)
{
    __asm { mov eax, 0x6F1DB690 }
    __asm { jmp eax }
}

}  // namespace

void __fastcall TailRedirectFieldReleaseIntoObserverUnhook(void* self)
{
    UnhookObserverHandle_6F1DB690(self, GetObserverHandle_6F1D2F20(self));
}
