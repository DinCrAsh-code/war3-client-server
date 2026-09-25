//============================================================================
//  CUnit batch-15 (slot 72 closure).  0x6F27BCF0 - resolve
//  CUnit::ResolveRef304()'s own result's +0x30 SOptionalHandleRef (the same
//  {t,b} pair shape QueryHandleField0x54 takes everywhere else in this
//  tree) and, if it resolves to something, tail-jump into a still-unnamed
//  callee (sub_6F2EEA30, no dump anywhere in this tree) with that object in
//  ecx and the caller's one stack dword passed through untouched.
//
//  Genuinely `this`-implicit, not a normal callable: no instruction here
//  ever sets ecx before the ResolveRef304 call, so `this` is already live
//  in ecx from the caller (0x6F2A40D0's own body, out of this batch's
//  scope) - the identical "custom register convention, real call site,
//  naked reproduction" case unit_callforhelprange.cpp documents.  `retn 4`
//  - the one stack dword is never read on any path, only ever forwarded.
//============================================================================
#include "unit.h"
#include "itemhandleresolve.h"

__declspec(naked) void ResolveRef304Field30_6F27BCF0()
{
    __asm
    {
        call    CUnit::ResolveRef304
        test    eax, eax
        jz      short bail
        lea     ecx, [eax+30h]
        mov     eax, [ecx+4]
        and     eax, [ecx]
        cmp     eax, 0FFFFFFFFh
        jz      short bail
        call    QueryHandleField0x54
        test    eax, eax
        jz      short bail
        mov     ecx, eax
        mov     eax, 06F2EEA30h
        jmp     eax
    bail:
        retn    4
    }
}
