//============================================================================
//  jscript-bin3's own worklist - three addresses unrelated to the
//  common.j/blizzard.j loading family (out of this round's own scope) and
//  to each other, each left a self-redirect thunk for its own reason below.
//============================================================================

#define SELF_REDIRECT(addr)          \
    {                                 \
        __asm { mov eax, addr }      \
        __asm { jmp eax }             \
    }

//  0x6F6F5530 - `retn 4` (__thiscall, 1 stack arg).  Reallocates a
//  fixed-stride (0x108-byte) element array tagged "AUWEDBFILE.C..." through
//  Storm_405/Storm_401/Storm_403, `rep movsd`-copying the overlap - a
//  WEDB-file record array's own grow-and-copy, unrelated to JASS.
__declspec(naked) void GrowWedbFileRecordArray(void* self, unsigned int)
    SELF_REDIRECT(06F6F5530h)

//  0x6F6FBA00 - `retn 4` (__thiscall, 1 stack arg).  Calls
//  sub_6F6F8E30 and, on success, walks two 8-byte-strided slots at
//  `this+0x1D4` calling sub_6F6FA510 on each - an unreconstructed sibling
//  cluster, unrelated to JASS.
__declspec(naked) int CheckTwoSlotPredicate(void* self, void*)
    SELF_REDIRECT(06F6FBA00h)

//  0x6F7E1860 - a 16-byte-aligning stack-probe variant (`ida_name`
//  __alloca_probe_16): rounds the requested size up to 16 bytes via
//  a carry trick, then tail-jumps into the real `_alloca_probe`/`__chkstk`
//  this repo already reconstructs at 0x6F7E1190 (Misc/chkstk.cpp) - CRT
//  boilerplate, not game code, the same class of address
//  docs/targets/JassVM_ModuleSweep.md's own chunk 0 already documents for
//  this module's neighbourhood.
__declspec(naked) void Alloca16ByteAligned()
    SELF_REDIRECT(06F7E1860h)

#undef SELF_REDIRECT
