//============================================================================
//  jscript-bin2's remaining assigned addresses outside the JASS VM module
//  proper (0x6F6Exxxx/0x6F71xxxx/0x6F73xxxx) - two are the linker's own
//  automatic Storm-ordinal import trampolines (nothing to compile into
//  src/, same as `SErrSetLastError`'s `jmp ds:__imp_Storm_465` -
//  docs/msvc-vc8-idioms.md), and the rest reach unreconstructed callees or
//  an `_except_handler4` frame out of this round's own budget.
//============================================================================

//  0x6F6FA510 - calls sub_6F6FA2D0, unreconstructed anywhere in this tree.
//  Shipped `retn 8` (two stack dwords) - declared `__stdcall` with two
//  dummy `int` args so this placeholder cleans the right number of bytes
//  if anything ever calls through it live (thunk_abi_audit.py's own
//  FATAL class).
__declspec(naked) void __stdcall Thunk_6F6FA510(int, int)
{
    __asm { mov eax, 06F6FA510h }
    __asm { jmp eax }
}

//  0x6F6FEFB0 - calls sub_6F6F8E30 (already a redirect,
//  LookupAndReleaseTerrainEntry) and sub_6F6FBA00, the latter with no
//  reconstruction anywhere in this tree.  Shipped `retn 12` (three stack
//  dwords).
__declspec(naked) void __stdcall Thunk_6F6FEFB0(int, int, int)
{
    __asm { mov eax, 06F6FEFB0h }
    __asm { jmp eax }
}

//  0x6F71B800 - a thin wrapper into sub_6F6C2620, which
//  Game/gamemainfatalmessage.cpp already documents as "out of this
//  session's scope (gamemain-batch-2 owns it)" under a message-box-shaped
//  signature that does not obviously match this call site's own register
//  usage - not chased further rather than guessed at.  Shipped `retn 4`
//  (one stack dword).
__declspec(naked) void __stdcall Thunk_6F71B800(int)
{
    __asm { mov eax, 06F71B800h }
    __asm { jmp eax }
}

//  0x6F71C0B0 - `_except_handler4`-framed lazy-singleton getter.
__declspec(naked) void Thunk_6F71C0B0()
{
    __asm { mov eax, 06F71C0B0h }
    __asm { jmp eax }
}

//  0x6F71CE10 - dispatches through four other singleton getters
//  (0x6F71BF00/0x6F71C0B0/0x6F71C530/0x6F71C410) and an unconfirmed
//  vtable slot - out of this round's own budget.
__declspec(naked) void Thunk_6F71CE10()
{
    __asm { mov eax, 06F71CE10h }
    __asm { jmp eax }
}

//  0x6F7367E0 - real definition now lives in Jass/jassloadscriptlikefile.cpp
//  (sub_6F7367E0, called for real from LoadJassLikeScriptOrWarn's own error
//  path) - removed the redundant/wrong-arity placeholder that used to be
//  here (thunk_abi_audit.py FATAL: this file's own `Thunk_6F7367E0()`
//  declared __cdecl/retn 0 against the shipped `retn 4`).
