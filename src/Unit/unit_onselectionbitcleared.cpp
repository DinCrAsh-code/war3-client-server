//============================================================================
//  0x6F333310 - CUnit::OnSelectionBitCleared, from the CUnit vtable closure
//  walk (cunit_agent2_worklist.md).  Was a naked redirect.
//
//  `GetGameUI(0, 0)` (gameui.h): with a live CGameUI already up, hand this
//  unit to 0x6F2F4930 - `mov ecx,[ecx+24Ch] / jmp sub_6F37D490`, a
//  thiscall(ecx=the CGameUI) member with one stack argument that reads its
//  own +0x24C selection-tracking array owner and tail-jumps into removing
//  this unit from it.  0x6F37D490 itself is out of this batch's slice of
//  the parallel worklist, so 0x6F2F4930 is declared here, not defined - the
//  call this function actually makes is the real, shipped one either way.
//  `GetGameUI(0, 0)` returning null (no UI up yet, e.g. very early in a
//  load) short-circuits the whole thing.
//============================================================================
#include "unit.h"
#include "gameui.h"

//  0x6F2F4930 - see above.  Declared as a member on an opaque type standing
//  in for CGameUI so the call compiles to the shipped `this`-in-ecx,
//  one-stack-argument shape without pretending to know CGameUI's own
//  layout past its own +0x24C field this reaches through.
struct SGameUISelectionTrackingHost
{
    void __thiscall RemoveUnitFromSelectionTracking(CUnit* unit);
};

void CUnit::OnSelectionBitCleared()
{
    SGameUI* ui = GetGameUI(0, 0);
    if (ui)
        ((SGameUISelectionTrackingHost*)ui)->RemoveUnitFromSelectionTracking(this);
}

//----------------------------------------------------------------------------
//  Link-completion thunk - 0x6F2F4930 is a real, separately-scored
//  reconstruction elsewhere (cunit_agent3's own SFieldAt0x24COwner::Forward,
//  tools/funcmap.py), but under a different class name, so the declaration
//  above never resolves against it. Same redirect-thunk pattern as this
//  repo's other out-of-scope callees.
//----------------------------------------------------------------------------
__declspec(naked) void __thiscall SGameUISelectionTrackingHost::RemoveUnitFromSelectionTracking(CUnit*)
{
    __asm { mov eax, 06F2F4930h }
    __asm { jmp eax }
}
