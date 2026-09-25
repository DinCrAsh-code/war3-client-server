//============================================================================
//  0x6F525E70 - QueryDefaultDisplayResolutionB3 (gamemain-batch-3).  Always
//  stamps both output words to 0xFFFF ("unknown") first; when `mode == 1`
//  it stops there and reports failure, otherwise it hands off to
//  sub_6F529E40 (0x6F529E40, a real per-adapter enumeration this session's
//  batch does not include - out-of-scope callee, kept as a naked thunk to
//  the shipped body per CLAUDE.md) to actually fill them in.
//============================================================================

//  0x6F529E40 - out of this session's batch; a real dump exists
//  (asm/GameMain_0x6F009850_calltree_asm.md) but its own further callees
//  (sub_6F52CF90, a vtable slot 5 dispatch, sub_6F52CAB0) are not, so it is
//  kept as a thunk to the shipped body rather than reconstructed here.
__declspec(naked) int __fastcall EnumerateDefaultDisplayResolution(
    unsigned short* outWidth, unsigned short* outHeight)
{
    __asm
    {
        mov eax, 06F529E40h
        jmp eax
    }
}

int __fastcall QueryDefaultDisplayResolutionB3(unsigned int mode,
                                               unsigned short* outWidth,
                                               unsigned short* outHeight)
{
    *outWidth = 0xFFFF;
    *outHeight = 0xFFFF;

    if (mode == 1)
        return 0;

    return EnumerateDefaultDisplayResolution(outWidth, outHeight);
}
