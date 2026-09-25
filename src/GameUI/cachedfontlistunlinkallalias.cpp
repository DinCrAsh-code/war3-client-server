//============================================================================
//  0x6F2FE230 - `jmp sub_6F2FE160`, one instruction.  A second address for
//  the identical TSExplicitList<CCachedFontRecord,12>::UnlinkAll body
//  (cachedfontlistunlinkall.cpp) - the same kind of linker-folded alias
//  0x6F2F83C0 is for 0x6F2F7880.  THUNK - nothing of its own to
//  reconstruct.
//============================================================================

//  An undefined `extern "C" sub_<addr>` leaves the real link
//  (`tools/link_check.py`) with an unresolved external - nothing on the
//  real link line defines a bare `sub_<addr>` symbol.  Written the
//  ADDR_THUNK way instead - one extra instruction against the dump's own
//  single `jmp`, which does not matter for a THUNK's score.
__declspec(naked) void CachedFontListUnlinkAllAlias()
{
    __asm { mov eax, 0x6F2FE160 }
    __asm { jmp eax }
}
