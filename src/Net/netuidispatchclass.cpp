//============================================================================
//  0x6F53F3C0 / 0x6F53F400 - the two byte-id wrappers over
//  DispatchSessionUiMessage.
//
//  Each takes an id in cl and builds the full message out of it, and the
//  only thing that differs between them is the class the id is OR'd into -
//  0xA0000 for the first and 0xB0000 for the second.  What those two
//  classes *are* is not established by anything in this call tree: CGameUI's
//  constructor registers ids 1-5, 0x84 and 0x85 through the first and 0x20
//  through the second, and nothing here says what numbering that is.  So
//  the two are named for the class each builds and nothing more is claimed.
//
//  `movzx ecx,cl` / `and ecx,0FFFFh` is the id widened from a byte and then
//  masked to sixteen bits, which is what an `unsigned char` parameter
//  combined into an `unsigned int` expression gives; the mask is redundant
//  after the `movzx` and the shipped code emits it anyway.
//
//  Own translation unit: both tail-jump into the real 0x6F53F350.
//============================================================================
#include "netdata.h"

void __fastcall DispatchSessionUiMessageA(unsigned char id, unsigned int arg1,
                                          void* arg2, int index)
{
    DispatchSessionUiMessage((id & 0xFFFFu) | 0xA0000u, arg1, arg2, index);
}

void __fastcall DispatchSessionUiMessageB(unsigned char id, unsigned int arg1,
                                          void* arg2, int index)
{
    DispatchSessionUiMessage((id & 0xFFFFu) | 0xB0000u, arg1, arg2, index);
}
