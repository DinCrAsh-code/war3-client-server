//============================================================================
//  0x6F332780 - cunit_agent5_worklist.md, CUnit closure BFS depth 1-2.
//  One instruction: `jmp sub_6F33AE40`.  Not the `mov eax,<addr>/jmp eax`
//  shape of a real thunk placeholder - a direct relative `jmp` to another
//  address in the same module - but nothing in this batch's worklist
//  calls it (grep says zero references outside this file), and the
//  callee itself (0x6F33AE40, no dump anywhere in this tree) ends both of
//  its own paths in a bare `retn` (0 stack bytes; the tail path it can
//  take, 0x6F4D95C0, does too), so the naked forwarding shape below is
//  exact either way and needs no real signature to be safe.
//============================================================================

__declspec(naked) void __stdcall Thunk_0x6F332780()
{
    __asm
    {
        mov eax, 06F33AE40h
        jmp eax
    }
}
