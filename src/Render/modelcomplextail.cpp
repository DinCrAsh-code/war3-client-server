//============================================================================
//  0x6F500AF0 - a one-instruction `jmp sub_6F500990` in the dump.
//
//  0x6F500990 itself is CModelComplex_'s own ~190-instruction sub-object
//  teardown (a chain of roughly fifteen more `lea ecx,[esi+offset]; call`
//  leaf destructors, none of which are in this batch's own dump or in any
//  of the 8 CItem-closure batches, plus several size-guarded Storm frees) -
//  deliberately left unreconstructed this pass rather than declaring
//  fifteen more unclaimed leaf thunks for a body this far from CItem's own
//  closure; see this batch's write-up for the full reasoning.  Reproduced
//  here the same way every other bare "jmp <address with no C++ name yet>"
//  redirect in this repo is, as a raw address rather than a symbol.
//============================================================================
__declspec(naked) void Thunk_0x6F500AF0()
{
    __asm
    {
        mov     eax, 06F500990h
        jmp     eax
    }
}
