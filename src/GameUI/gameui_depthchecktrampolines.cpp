//============================================================================
//  0x6F332D20/0x6F332DE0 - two of CAbility slot 165's own closure
//  (asm/sub_6F02E160_0x6F02E160_calltree_asm.md), cability_depth5_closure_
//  worklist.json batch 3.  Both are guarded forwarders into CGameUI
//  rendering entry points (0x6F300AB0/0x6F300C40) that this closure does
//  not reach (below depth<=5 from this batch's own roots) - declared
//  here as generic stack-passthrough thunks, exact argument *types* and
//  *names* unrecovered (this file does not claim what they mean), exact
//  argument *count* read directly off each callee's own `retn`.
//----------------------------------------------------------------------------
//  0x6F300AB0/0x6F300C40 - below this closure's depth; declared, not
//  reconstructed.  __thiscall(CGameUI*), 10/4 stack dwords respectively -
//  counted from each one's own `retn` (0x28/0x10).
//============================================================================
#include "gameui.h"

//  A minimal stand-in receiver, the same reasoning ability_slots.cpp's own
//  CAbilityLevelRowRaw note gives: only needs the mangled name to match,
//  not the real layout of whatever this really is (CGameUI itself, most
//  likely, given GetGameUI's own result feeds it).
class CGameUIRenderDepthCheckOwner
{
public:
    int RenderDepthCheckA(int a0, int a1, int a2, int a3, int a4, int a5,
                          int a6, int a7, int a8, int a9);
    int RenderDepthCheckB(int a0, int a1, int a2, int a3);
};

__declspec(naked) int CGameUIRenderDepthCheckOwner::RenderDepthCheckA(
    int, int, int, int, int, int, int, int, int, int)
{
    __asm { mov eax, 0x6F300AB0 }
    __asm { jmp eax }
}

__declspec(naked) int CGameUIRenderDepthCheckOwner::RenderDepthCheckB(
    int, int, int, int)
{
    __asm { mov eax, 0x6F300C40 }
    __asm { jmp eax }
}

//----------------------------------------------------------------------------
//  0x6F332D20 - guarded ten-argument forward: skip entirely when this
//  object's own +0x248 bit 0x200 is set, otherwise resolve the (already
//  running, not newly created) GameUI and forward `self`/`extra` plus
//  eight more caller arguments to GameUIRenderDepthCheckA.  __fastcall
//  (self in ecx, extra in edx) + 8 stack dwords, `retn 20h`.
//----------------------------------------------------------------------------
int __fastcall GameUIRenderDepthCheckTrampolineA(void* self, int extra, int a0,
                                                 int a1, int a2, int a3, int a4,
                                                 int a5, int a6, int a7)
{
    if ((*(unsigned int*)((char*)self + 0x248) & 0x200) != 0)
        return 0;

    CGameUIRenderDepthCheckOwner* ui = (CGameUIRenderDepthCheckOwner*)GetGameUI(0, 0);
    if (ui == 0)
        return 0;
    return ui->RenderDepthCheckA(a0, a1, a2, a3, a4, a5, extra, (int)self, a6, a7);
}

//----------------------------------------------------------------------------
//  0x6F332DE0 - the same shape, unconditional and four arguments forward.
//  __fastcall(self in ecx, extra in edx) + 2 stack dwords, `retn 8`.
//----------------------------------------------------------------------------
int __fastcall GameUIRenderDepthCheckTrampolineB(void* self, int extra, int a0,
                                                 int a1)
{
    CGameUIRenderDepthCheckOwner* ui = (CGameUIRenderDepthCheckOwner*)GetGameUI(0, 0);
    if (ui == 0)
        return 0;
    return ui->RenderDepthCheckB(a1, a0, extra, (int)self);
}
