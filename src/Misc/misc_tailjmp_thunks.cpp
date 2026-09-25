//============================================================================
//  Six one-instruction `jmp <target>` thunks from sub_6F29ED60's own call
//  tree, each redirecting to a function this session did not reconstruct.
//  A bare `jmp` does not touch any argument register or the stack, so its
//  own bytes do not depend on the target's calling convention - declared
//  `__declspec(naked)` with no parameters is enough to reproduce them
//  exactly regardless of what the (undefined) target's real signature is.
//============================================================================

extern "C" void sub_6F5FD3B0();
extern "C" void sub_6F5FF380();
extern "C" void sub_6F7C0CD0();
extern "C" void sub_6F7C2430();
extern "C" void sub_6F7C24C0();
extern "C" void sub_6F7C3B30();

__declspec(naked) void TailJmp_6F5FD480() { __asm jmp sub_6F5FD3B0 }
__declspec(naked) void TailJmp_6F5FFED0() { __asm jmp sub_6F5FF380 }
__declspec(naked) void TailJmp_6F7C22C0() { __asm jmp sub_6F7C0CD0 }
__declspec(naked) void TailJmp_6F7C2F30() { __asm jmp sub_6F7C2430 }
__declspec(naked) void TailJmp_6F7C2F70() { __asm jmp sub_6F7C24C0 }
__declspec(naked) void TailJmp_6F7C4BF0() { __asm jmp sub_6F7C3B30 }
