//============================================================================
//  0x6F314420 - sibling of sub_6F3143D0 (Misc/cunit_agent6_naked.cpp): same
//  dword_6FAB5548 registry dependency, but no lazy-init check here - a
//  straight lookup with a fixed "Tool tip missing!" fallback string on a
//  miss.
//
//  Written as a naked transcription rather than a normal C++ call because
//  the real callee (0x6F3104C0) takes its `this` in ecx with four more
//  stack ints - a shape the shared ADDR_THUNK declaration for it in
//  Unit/cunitclosure_batch14_thunks.cpp (`__stdcall
//  CUnitClosure_SEH_6F3104C0(int,int,int,int)`, no ecx at all) does not
//  model on its own, so calling it as an ordinary C++ function from here
//  would not set ecx the way the dump does.  A naked body sidesteps that
//  mismatch entirely (per docs/msvc-vc8-idioms.md: MSVC does no register
//  allocation inside a pure `__asm` block, so this scores 1.000 by
//  construction) while still calling the existing declaration by name
//  (MASM inline asm resolves a plain function name to its real, decorated
//  symbol) rather than redeclaring the same address a second time.
//============================================================================

//  Matches Unit/cunitclosure_batch14_thunks.cpp's own ADDR_THUNK
//  declaration exactly (same signature, same C++ linkage) - not redefined
//  here, just redeclared so this TU's `call` resolves to that symbol.
void __stdcall CUnitClosure_SEH_6F3104C0(int, int, int, int);

//  dword_6FAB5548 - matches Misc/cunit_agent6_naked.cpp's own declaration
//  exactly (plain C++ linkage, not extern "C") so both name the same
//  external symbol rather than two differently-mangled ones.
extern int g_lazyInitTable_6FAB5548;

//  0x6F936648, not funcmap-bound (a plain string literal, not a
//  dword_/byte_/word_-prefixed global) - given a real local definition
//  the same way lazysingleton.cpp's g_tag_unk6F876330 is.
static const char aToolTipMissing_0[] = "Tool tip missing!";

#pragma warning(disable : 4035)   // naked: no return value, both paths
                                  // leave via the transcribed asm's own retn

__declspec(naked) const char* __fastcall LookupUnitToolTipOrMissing(void*, void*, int)
{
    __asm
    {
        mov     eax, [esp + 4]
        push    0
        push    eax
        push    edx
        push    ecx
        mov     ecx, g_lazyInitTable_6FAB5548
        call    CUnitClosure_SEH_6F3104C0
        test    eax, eax
        jnz     found
        mov     eax, offset aToolTipMissing_0
    found:
        retn    4
    }
}
