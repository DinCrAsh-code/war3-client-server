//============================================================================
//  GameMain closure (gamemain-batch-1) - naked redirects to real, unhooked
//  shipped bodies, one per address, each genuinely below this session's
//  scope:
//
//  - 0x6F62ABD0/0x6F62AC30 are themselves one-instruction tail jumps in the
//    shipped binary (`jmp sub_6F630440`/`jmp sub_6F631290`); self-redirect
//    is the established pattern for that shape (see
//    Net/netproviderltcp_notify.cpp's own 0x6F62ABF0, right next door).
//  - 0x6F7AD920/0x6F7AD910 are the same one-instruction-tail-jump shape
//    (`jmp sub_6F7AD8D0`/`jmp sub_6F7AD8E0`).
//  - 0x6F4674F0/0x6F467500 each call one empty nullsub and tail-jump to a
//    second (`call nullsub_18; jmp nullsub_35` and
//    `call nullsub_17; jmp nullsub_36`) - net effect is a no-op either way,
//    self-redirected rather than spelled out since the two nullsub
//    addresses are themselves out of this session's scope.
//============================================================================

__declspec(naked) void NoOpAlias_6F62ABD0()
{
    __asm { mov eax, 0x6F62ABD0 }
    __asm { jmp eax }
}

__declspec(naked) void NoOpAlias_6F62AC30()
{
    __asm { mov eax, 0x6F62AC30 }
    __asm { jmp eax }
}

__declspec(naked) void NoOpAlias_6F7AD920()
{
    __asm { mov eax, 0x6F7AD920 }
    __asm { jmp eax }
}

__declspec(naked) void NoOpAlias_6F7AD910()
{
    __asm { mov eax, 0x6F7AD910 }
    __asm { jmp eax }
}

__declspec(naked) void NoOpAlias_6F4674F0()
{
    __asm { mov eax, 0x6F4674F0 }
    __asm { jmp eax }
}

__declspec(naked) void NoOpAlias_6F467500()
{
    __asm { mov eax, 0x6F467500 }
    __asm { jmp eax }
}
