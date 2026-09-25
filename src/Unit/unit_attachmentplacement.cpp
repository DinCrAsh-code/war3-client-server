//============================================================================
//  0x6F278D40 - CUnit's own override of vtable slot 104 (+0x1A0), which
//  CSelectable declares (selectable_attachrefresh.cpp) and CItem inherits
//  unchanged.  Same job - re-place this object's visual attachments at its
//  current world position - but CUnit carries a *third* attachment slot
//  (m_handle278, unit.h's own +0x278) that CSelectable's two-slot version
//  does not, and folds two more differences in along the way:
//
//    * position comes from GetOffsetPosition() (widget.h's plain, non-slot
//      member that folds the module's fixed world offset into slot 56's
//      answer) called directly, not through the vtable[0xE0] indirection
//      CSelectable's own body uses;
//    * when m_attachmentRefreshFlag is *not* zero, the shipped code takes a
//      wholly different branch from CSelectable's "skip the Z write,
//      still call GetConfigFloatHW and throw it away" shape: it re-checks
//      slot 58's own answer (Method_0xE8()) and, only when that answer is
//      not positive, re-zeroes every present attachment's Z with a literal
//      0.0f/onWalkable=0 rather than reusing the position-fetch's own
//      onWalkable flag or the config lookup at all;
//    * a trailing block nothing in CSelectable's version has: when the
//      unit's own footprint type carries "SelCircleOnWater"
//      (footprintselcircleonwater.cpp), every present slot also gets two
//      more per-record bits set through SetVisualAttachmentFlags
//      (stormsingleton_attach.cpp / stormattachments.cpp's
//      SetAttachmentFlagBit9/10 - CLAUDE.md's "do not invent fields the
//      call tree does not read": nothing here says what those two bits
//      mean beyond their position).
//
//  The lingering-FPU-value idiom CSelectable's own body already has (a
//  pushed 0.0 surviving across a branch to feed a later `fstp`) shows up
//  twice more here, for the same reason: `isNonPositive`'s own `fldz` stays
//  on the x87 stack all the way to whichever `fstp` first consumes or
//  discards it - the explicit `0.0f` GetConfigFloatHW's own "throw it away"
//  branch reaches, or the bare `fstp st` at the very end of the
//  m_attachmentRefreshFlag branch when neither condition needed it.
//
//  Own translation unit: five real calls out of it (GetOffsetPosition,
//  Method_0xE8, GetConfigFloatHW, GetSelCircleOnWaterFlag and the Storm
//  wrappers), none to each other.
//============================================================================
#include "unit.h"
#include "fvec3.h"
#include "../Pathfinding/footprinttype.h"

//  0x6F00D9A0 / 0x6F00D9C0 / 0x6F00DA00 - stormsingleton_attach.cpp.
void __fastcall SetVisualAttachmentZ(int index, int onWalkable, float z);
void __fastcall SetVisualAttachmentPosition(int index, const FVec3* pos);
void __fastcall SetVisualAttachmentFlags(int index, int bit9On, int bit10On);

//  0x6F009E30 - configgates.cpp.  A second, hardware-float copy of
//  GetConfigFloat's body; the answer comes back in st0.
float __fastcall GetConfigFloatHW(const char* section, const char* key, int index);

//  the "none" sentinel every attachment-slot field is compared against.
extern const unsigned int g_unk6F932954;

void CUnit::RefreshAttachmentPlacement()
{
    if (m_attachmentSlot1 == g_unk6F932954 &&
        m_handle278 == g_unk6F932954 &&
        m_attachmentSlot2 == g_unk6F932954)
        return;

    FVec3 pos;
    int onWalkable = 0;
    GetOffsetPosition(&pos, 1, -1, (int)&onWalkable);

    //  Slot 58's own answer, folded straight into a bool.  Spelled as
    //  `<=` on purpose - CLAUDE.md/docs/msvc-vc8-idioms.md's own
    //  "`!(a > b)` and `a <= b` are different instructions" note: `<=`
    //  compiles to `fcom`/`fnstsw`/`test ah,1`, matching the dump exactly,
    //  where `!(0.0f < x)` compiled to a different `fcomp`/`test ah,65`
    //  shape (the same *result* for non-NaN inputs, a different
    //  instruction stream). The pushed 0.0 is what lingers on the FPU
    //  stack for the rest of this function, per the file comment above.
    int isNonPositive = (Method_0xE8() <= 0.0f);

    if (m_attachmentRefreshFlag == 0)
    {
        float z = onWalkable
            ? GetConfigFloatHW("SelectionCircle", "ImageWalkableZOffset", 0)
            : 0.0f;

        if (m_attachmentSlot1 != g_unk6F932954)
            SetVisualAttachmentZ((int)m_attachmentSlot1, onWalkable, z);
        if (m_handle278 != g_unk6F932954)
            SetVisualAttachmentZ((int)m_handle278, onWalkable, z);
        if (m_attachmentSlot2 != g_unk6F932954)
            SetVisualAttachmentZ((int)m_attachmentSlot2, onWalkable, z);
    }
    else if (isNonPositive)
    {
        if (m_attachmentSlot1 != g_unk6F932954)
            SetVisualAttachmentZ((int)m_attachmentSlot1, 0, 0.0f);
        if (m_handle278 != g_unk6F932954)
            SetVisualAttachmentZ((int)m_handle278, 0, 0.0f);
        if (m_attachmentSlot2 != g_unk6F932954)
            SetVisualAttachmentZ((int)m_attachmentSlot2, 0, 0.0f);
    }

    if (m_attachmentSlot1 != g_unk6F932954)
        SetVisualAttachmentPosition((int)m_attachmentSlot1, &pos);
    if (m_handle278 != g_unk6F932954)
        SetVisualAttachmentPosition((int)m_handle278, &pos);
    if (m_attachmentSlot2 != g_unk6F932954)
        SetVisualAttachmentPosition((int)m_attachmentSlot2, &pos);

    if (GetSelCircleOnWaterFlag(m_footprintType))
    {
        if (m_attachmentSlot1 != g_unk6F932954)
            SetVisualAttachmentFlags((int)m_attachmentSlot1, 1, 0);
        if (m_handle278 != g_unk6F932954)
            SetVisualAttachmentFlags((int)m_handle278, 1, 0);
        if (m_attachmentSlot2 != g_unk6F932954)
            SetVisualAttachmentFlags((int)m_attachmentSlot2, 1, 0);
    }
}
