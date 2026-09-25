//============================================================================
//  0x6F35B970 - one of SSelectionVisual::Detach's own callees (0x6F379D10,
//  selectionvisualrelease.cpp - itself entirely unhooked shipped code, so
//  this address is reconstructed only for its own scored entry).  353
//  instructions carrying a full `__except_handler4`-shaped SEH frame this
//  toolchain cannot reproduce under the fixed `/GS- /EHs-c-` build flags
//  (docs/msvc-vc8-idioms.md).  `retn 4` - one stack argument, `this` in
//  ecx.
//============================================================================

//  Guarded at its one call site by `if (target != this->m_current) ...`,
//  which is as much as this call tree establishes about what it does with
//  `target` - named for that rather than left `Method`.
struct SSelectionVisualDetail
{
    void ReleasePreviousTarget(int target);
};

__declspec(naked) void SSelectionVisualDetail::ReleasePreviousTarget(int)
{
    __asm
    {
        mov     eax, 06F35B970h
        jmp     eax
    }
}
