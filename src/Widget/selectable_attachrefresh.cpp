//============================================================================
//  0x6F2C7600 - CSelectable's vtable slot 104 (+0x1A0), inherited unchanged
//  by CItem.
//
//  Re-place this selectable's two visual attachments (the selection-circle
//  decal and the walkable-Z marker, selectable.h's +0x44/+0x48) at the
//  object's current world position.  A previous session left this root
//  deliberately TODO as "visual-attachment bookkeeping, not item-placement
//  logic" (docs/targets/CItem__Method_0x1A0.md); this session's sweep of
//  all three vtables is what brings it back into scope.
//
//  Nothing happens at all when neither attachment exists, which is the
//  early-out at the top - and note it is `&&` over the two -1 sentinels
//  separately, not the `(a & b) == -1` shape the {handle, typeTag} pairs in
//  this codebase use, because these two are independent slots and not a
//  pair.
//
//  Own translation unit: four real calls out of it, none to each other.
//============================================================================
#include "selectable.h"
#include "fvec3.h"

//  0x6F00D9A0 / 0x6F00D9C0 - stormsingleton_attach.cpp.
void __fastcall SetVisualAttachmentZ(int index, int onWalkable, float z);
void __fastcall SetVisualAttachmentPosition(int index, const FVec3* pos);

//  0x6F009E30 - configgates.cpp.  A second, hardware-float copy of
//  GetConfigFloat's body; the answer comes back in st0.
float __fastcall GetConfigFloatHW(const char* section, const char* key, int index);

//  slot 56 (+0xE0) - CWidget::GetPosition, reached through the vtable
//  because CItem overrides it and this body is CSelectable's.
typedef FVec3* (__thiscall *Slot0xE0Fn)(void* self, FVec3* out, int wantGround,
                                        int a3, int* onWalkable);

void CSelectable::RefreshAttachmentPlacement()
{
    if (m_attachmentSlot1 == (unsigned int)-1 &&
        m_attachmentSlot2 == (unsigned int)-1)
        return;

    //  The zero really is in the source: slot 56 writes this flag, and the
    //  shipped code still stores 0 into the slot beforehand.
    FVec3 pos;
    int onWalkable = 0;
    ((Slot0xE0Fn)(*(void***)this)[0xE0 / 4])(this, &pos, 1, -1, &onWalkable);

    //  A hardware float held in st0 across the branch, which is what the
    //  shipped `fldz` on the else edge is - a zero pushed onto the FPU
    //  stack rather than stored, so both edges leave the value in the same
    //  place for the single `fstp` that follows.
    float z = onWalkable
        ? GetConfigFloatHW("SelectionCircle", "ImageWalkableZOffset", 0)
        : 0.0f;

    if (m_attachmentSlot1 != (unsigned int)-1)
    {
        if (m_attachmentRefreshFlag == 0)
            SetVisualAttachmentZ((int)m_attachmentSlot1, onWalkable, z);
        else
            //  The answer is computed and thrown away - the shipped code
            //  really does call it here and follow it with a bare
            //  `fstp st`, which is MSVC's own discard of an unused float
            //  return.  Only this first attachment has the branch; the
            //  second one below simply skips the Z write, which is the
            //  asymmetry the shipped code has and not a transcription slip.
            GetConfigFloatHW("SelectionCircle", "ImageWalkableZOffset", 0);

        SetVisualAttachmentPosition((int)m_attachmentSlot1, &pos);
    }

    if (m_attachmentSlot2 != (unsigned int)-1)
    {
        if (m_attachmentRefreshFlag == 0)
            SetVisualAttachmentZ((int)m_attachmentSlot2, onWalkable, z);

        SetVisualAttachmentPosition((int)m_attachmentSlot2, &pos);
    }
}
