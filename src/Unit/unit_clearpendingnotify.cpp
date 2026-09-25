//============================================================================
//  0x6F284B40 - CUnit's own override of vtable slot 32 (+0x80), replacing
//  CWar3Image's own body (selectable.cpp) with a much larger one that
//  still opens by calling the base version directly (not through the
//  vtable - `call sub_6F26BEA0` with no reload of the vtable pointer),
//  the same way CItem::ClearPendingAndNotify (item.cpp) does for its own,
//  much smaller override at the same slot.
//
//  If m_flags bit 0 is clear, the whole function is a no-op - not even
//  the base call runs.
//
//  Otherwise: call the base, clear m_flags5C bit 3, then branch on
//  m_flags5C bit 8:
//
//    * clear (the common path): DispatchPositionNotifyState(1)
//      (0x6F283BC0, unit.h - the m_notifyHandle25C state machine's own
//      dispatcher, already named and thunked by an earlier session),
//      publish visibility (SetAttachment274Visible(1)), re-run
//      AddFootprint/SetFlagBit2 with every argument `1`, notify the Storm
//      terrain singleton once per handle field slot 98 (Method_0x188) and
//      slot 114 (Method_0x1C8) each gate, then the *exact* any/allies/
//      enemies cascade CGameUiWar3::UpdatePauseHighlightsForCursorReset
//      (gameuicursorreset.cpp) already establishes -
//      `IsEventKeyRegistered` against the same three `kUiOption*` values
//      (frame.h/gameuicursorreset.cpp), short-circuited through `any` -
//      feeding `ApplyPauseHighlightToWidget(this, showAllies, showEnemies)`
//      (gameuihighlightsweep.cpp) when either is set; then grant the
//      ability `GetConstant_6F082470()` names (FindAbility) and, if
//      found, run a follow-up (sub_6F2E5030, out of scope - see below);
//    * set: RemoveFootprint(0, 1) instead of any of the above - the
//      `1` reused here is the *same* stack push the common path's own
//      DispatchPositionNotifyState(1) call would have consumed, left on
//      the stack across the branch (the shipped compiler's own
//      push-before-the-test idiom, not something this reconstruction
//      needs to reproduce byte-for-byte to be behaviourally right).
//
//  Both arms rejoin to re-derive this unit's own world position through
//  slot 46's handle object (CPathRef::SmartPtrToWorldVec3) and hand it to
//  Reposition with a fixed argument pattern, then tail-call slot 45
//  (Method_0xB4 / RefreshOwnershipHighlight).
//
//  IMPORTANT lesson from this file's own first draft: the two calls in the
//  "look up a small resource" paragraph above were *not* new, unnamed
//  leaves - they are 0x6F62B170 (`IsEventKeyRegistered`,
//  Frame/eventkeyquery.cpp) and 0x6F37D0D0 (`ApplyPauseHighlightToWidget`,
//  GameUI/gameuihighlightsweep.cpp), both already real, fully-reconstructed
//  functions reused here for a second, unrelated purpose (a unit clearing
//  its pending state re-evaluates the pause-highlight it may currently be
//  wearing). A first pass invented brand-new naked thunks under made-up
//  names for both before `funcmap_lint.py` caught the resulting duplicate
//  FUNCTIONS/BEHAVIOUR keys - textbook case of CLAUDE.md's own "grep
//  before writing" rule, missed here because the grep only covered
//  `src/` file *comments*, not `tools/funcmap.py`'s FUNCTIONS table
//  itself, which is where the two addresses' real names actually lived.
//
//  One leaf reached here has no dump chased past its own call site's push
//  count, per this pass's own "do not go deep" scope: the found-ability
//  follow-up (sub_6F2E5030, its own two callees unestablished).
//============================================================================
#include "unit.h"
#include "game.h"                    // CWorldVec3, CPathRef
#include "stormnotifysingleton.h"    // NotifyStormSingleton - 0x6F00D990
#include "frame.h"                   // IsEventKeyRegistered - 0x6F62B170

//  0x6F082470 - already reconstructed, misc_rawcode_getters.cpp.
unsigned int GetConstant_6F082470();

//  the "none" sentinel every attachment-slot field is compared against -
//  unit_attachmentplacement.cpp's own copy of the same extern.
extern const unsigned int g_unk6F932954;

//  0x6F37D0D0 - already reconstructed, GameUI/gameuihighlightsweep.cpp.
void __fastcall ApplyPauseHighlightToWidget(CWidget* self, int checkAllies,
                                            int checkEnemies);

//  the same three option ids gameuicursorreset.cpp's own cascade tests -
//  0x002/0x112/0x113, confirmed a match instruction-for-instruction
//  against this override's own three LookupNotifyResource-shaped calls.
enum
{
    kUiOptionHighlightAny     = 0x002,
    kUiOptionHighlightAllies  = 0x112,
    kUiOptionHighlightEnemies = 0x113,
};

//  0x6F0787D0 - SUnitAbilityHost::FindAbility, already reconstructed
//  (unit_findability.cpp); `this` reinterpreted the same way that file's
//  own call sites do.
struct SUnitAbilityHost
{
    void* FindAbility(int rawcode, int a, int b, int c, int d);
};

//  0x6F283BC0 - the m_notifyHandle25C state machine's own dispatcher
//  (citemclosure_batchL_thunks.cpp's own note on this address); real
//  caller now, `this` in ecx and one stack argument, `retn 4`.
__declspec(naked) void CUnit::DispatchPositionNotifyState(int arg)
{
    __asm
    {
        mov     eax, 06F283BC0h
        jmp     eax
    }
}

//  the one still-unnamed leaf this override reaches with no dump chased
//  past its own call site's push count (two undumped callees of its
//  own).
__declspec(naked) void CUnit::OnAbilityGrantedForNotify_6F2E5030()
{
    __asm
    {
        mov     eax, 06F2E5030h
        jmp     eax
    }
}

void CUnit::ClearPendingAndNotify()
{
    if ((m_flags & 1) == 0)
        return;

    CWar3Image::ClearPendingAndNotify();
    m_flags5C &= ~8u;

    if ((m_flags5C & 0x100) == 0)
    {
        DispatchPositionNotifyState(1);
        SetAttachment274Visible(1);
        AddFootprint(1, 1, 1);
        SetFlagBit2(1);

        if (Method_0x188() != 0 && m_attachmentSlot1 != g_unk6F932954)
            NotifyStormSingleton(m_attachmentSlot1, 1);
        if (Method_0x1C8() != 0 && m_handle278 != g_unk6F932954)
            NotifyStormSingleton(m_handle278, 1);

        int any = IsEventKeyRegistered((void*)kUiOptionHighlightAny);
        int showAllies  = (any != 0) || (IsEventKeyRegistered((void*)kUiOptionHighlightAllies) != 0);
        int showEnemies = (any != 0) || (IsEventKeyRegistered((void*)kUiOptionHighlightEnemies) != 0);
        if (showAllies != 0 || showEnemies != 0)
            ApplyPauseHighlightToWidget(this, showAllies, showEnemies);

        void* ability = ((SUnitAbilityHost*)this)
                            ->FindAbility(GetConstant_6F082470(), 1, 0, 1, 1);
        if (ability != 0)
            OnAbilityGrantedForNotify_6F2E5030();
    }
    else
    {
        RemoveFootprint(0, 1);
    }

    CWorldVec3 pos = ((CPathRef*)GetHandleObject())->SmartPtrToWorldVec3();
    Reposition(&pos.m_x, &pos.m_y, 1, 1, -1, 0, 0, 0, -1, 0);
    Method_0xB4();
}
