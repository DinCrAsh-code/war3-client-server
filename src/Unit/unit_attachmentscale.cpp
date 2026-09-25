//============================================================================
//  0x6F26F4B0 - CUnit's vtable slot 105 (SetAttachmentScale).  A naked
//  redirect in unit_slotthunks.cpp until now.
//
//  Run CSelectable's own body first, then - only when this unit still holds
//  a selection-circle attachment - push a second, derived scale at that
//  attachment: the "SelectionCircle"/"AllyOffset" config float plus the
//  caller's scale, doubled, on both axes.
//
//  Three shapes are the shipped code's and not a choice:
//
//  * **the base call is qualified, not virtual.**  `call 0x6F2C76D0` is a
//    direct call, so this is `CSelectable::SetAttachmentScale(scale)` and
//    not a dispatch through the slot it is itself in - which would recurse.
//  * **the doubling is written `* 2.0f` and the whole thing is one
//    expression.**  MSVC turns a multiply by two into `fadd st, st` by
//    itself, which is the shipped instruction; what it will not do is skip
//    the store when the intermediate is given a name - a `float sum = ...;
//    scale = sum + sum;` spelling spills and reloads twice where the
//    shipped code does it once.
//  * **the result is written back over the parameter.**  `fstp [esp+arg_0]`
//    / `fld [esp+arg_0]` is a store into the incoming argument slot and a
//    reload from it, which is what assigning to a by-value parameter looks
//    like and what a fresh local does not (a local gets its own slot).
//
//  The two-element array is one value stored twice - `fst` then `fstp` into
//  adjacent slots, one load feeding both.
//
//  Own translation unit: CSelectable::SetAttachmentScale
//  (selectable_vtable.cpp) and both config gates are real calls out of it.
//============================================================================
#include "unit.h"
#include "selectable.h"

//  configgates.cpp's, under the names that file already gives them - a
//  local re-declaration under a different name compiles, links and
//  silently costs the call site its match.
float __fastcall GetConfigFloatHW(const char* section, const char* key,
                                  int index);
void __fastcall AttachmentSetScale(unsigned int slot, const float* scaleXY);

//  The "no attachment" sentinel unit_playertable.cpp already reaches.
extern const unsigned int g_unk6F932954;

void CUnit::SetAttachmentScale(float scale)
{
    CSelectable::SetAttachmentScale(scale);

    if (m_handle278 != g_unk6F932954)
    {
        scale = (GetConfigFloatHW("SelectionCircle", "AllyOffset", 0) + scale) * 2.0f;

        float scaleXY[2] = { scale, scale };
        AttachmentSetScale(m_handle278, scaleXY);
    }
}
