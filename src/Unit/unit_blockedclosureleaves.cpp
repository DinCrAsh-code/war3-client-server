//============================================================================
//  Naked redirects for four depth-1 closure leaves of CUnit's five
//  confirmed-permanent-blocker slots (91, 103, 115, 116, 117 -
//  docs/targets/CUnit__vtable.md's "CUnit batch-6" note).  Each of these
//  four opens with the full __except_handler4 SEH frame (security cookie,
//  scope-table push) this repo's fixed /GS- /EHs-c- build cannot
//  reproduce - the same ceiling unit_flagbit8.cpp's own SetFlagBit8 hits,
//  and the reason slots 91/115/116/117's own roots (RefreshPlacementRadius,
//  Method_0x1CC/0x1D0/0x1D4, unit_slotthunks.cpp) stay thunked rather
//  than reconstructed. None is specific to one slot's closure; all four
//  are quick-confirmed here (not re-derived - CUnit batch-8's own brief)
//  so the worklist entries reaching them have a real declaration and
//  funcmap row instead of sitting `TODO`.
//
//  Every retn byte count is read directly off each function's own
//  `asm/` dump rather than guessed from its call site.
//============================================================================
#define ADDR_THUNK(sig, addr)          \
    __declspec(naked) sig              \
    {                                  \
        __asm { mov eax, addr }        \
        __asm { jmp eax }              \
    }

//  All four take `this` in ecx (thiscall) and no other register argument;
//  a single throwaway receiver type gets every one of them a correct
//  thiscall declaration without claiming a real class name none of these
//  four dumps establishes.
struct SCUnitBlockedClosureLeaves
{
    void ReleaseAndRederivePlacementRadius();   // 0x6F283670, `retn 0`
    void Slot116ClosureLeaf(void* a);           // 0x6F27F400, `retn 4`
    void Slot117ClosureLeaf(void* a);           // 0x6F27F460, `retn 4`
    void Slot101ClosureLeaf27A780();            // 0x6F27A780, `retn 0`
};

//  0x6F283670 (`retn 0`) - RefreshPlacementRadius's (slot 91) own blocking
//  callee: releases the old placement-radius CFloat pair and re-derives a
//  fresh one behind the SEH-protected temporaries.
ADDR_THUNK(void SCUnitBlockedClosureLeaves::ReleaseAndRederivePlacementRadius(), 0x6F283670)

//  0x6F27F400 (`retn 4`) - one stack argument, reached from slot 116's own
//  root (Method_0x1D0, 0x6F2979E0) before that root's own SEH frame gets
//  far enough to call it.
ADDR_THUNK(void SCUnitBlockedClosureLeaves::Slot116ClosureLeaf(void* a), 0x6F27F400)

//  0x6F27F460 (`retn 4`) - one stack argument, slot 117's own root's
//  (Method_0x1D4, 0x6F297B30) mirror of the leaf above.
ADDR_THUNK(void SCUnitBlockedClosureLeaves::Slot117ClosureLeaf(void* a), 0x6F27F460)

//  0x6F27A780 (`retn 0`) - reached from slot 101's own root
//  (EnsureSelectionCircle, 0x6F28DCF0); does not itself carry the SEH
//  frame, but shares slot 103's own blocking descendant (sub_6F330AF0,
//  plain-/GS-cookie-prologue, equally unreproducible under this build's
//  fixed /GS- per docs/msvc-vc8-idioms.md) two calls deep - quick-confirmed
//  alongside the SEH family rather than reconstructed on a guess at that
//  callee's own retn.
ADDR_THUNK(void SCUnitBlockedClosureLeaves::Slot101ClosureLeaf27A780(), 0x6F27A780)

#undef ADDR_THUNK
