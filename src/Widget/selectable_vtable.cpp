//============================================================================
//  CSelectable's own vtable slots.  See selectable.h for the declarations
//  and docs/targets/CWidget_CSelectable_CItem__vtables.md for the survey.
//
//  Two address neighbourhoods here, 0x6F2C7xxx and 0x6F266xxx, which is the
//  same split CSelectable::CSelectable (0x6F266C30) and
//  CSelectable::Reposition (0x6F266C80) already sit either side of - the
//  shipped source had this class spread over the same two modules CWidget
//  is.  None of these functions calls another directly (Deactivate reaches
//  slot 97 through the vtable, not by name), so one translation unit costs
//  nothing.
//============================================================================
#include "selectable.h"

//  All from configgates.cpp.
float __fastcall GetConfigFloatHW(const char* section, const char* key, int index);
void __fastcall AttachmentRelease(unsigned int slot);
void __fastcall AttachmentSetVisible(unsigned int slot, int on);
void __fastcall AttachmentSetScale(unsigned int slot, const float* scaleXY);
void __fastcall ReleaseSelectionVisualObject(void* visual);
int  __fastcall SelectableSlot0x19CGate(void* self);

//----------------------------------------------------------------------------
//  0x6F2C73F0 - slot 7.  '+s3w' against CWidget's own '+w3w'.
//----------------------------------------------------------------------------
unsigned int CSelectable::GetAgileTypeId() const
{
    return 0x2B773373;
}

//----------------------------------------------------------------------------
//  0x6F2C73A0 - slot 22.
//----------------------------------------------------------------------------
const char* CSelectable::GetClassName()
{
    return "CSelectable";
}

//----------------------------------------------------------------------------
//  0x6F2C7410 - slot 13.  The base class's teardown first (a direct
//  `call sub_6F2ABEA0`, not a virtual dispatch - this *is* the override, so
//  going through the vtable would recurse), then let go of both visual
//  attachment slots, then tail-call slot 97 through the vtable.
//
//  Slot 97 is reached virtually and not as `ReleaseSelectionVisual()`
//  because CItem overrides neither this slot nor 97 - but CDestructable and
//  CUnit's own vtables (user_knowledge.json) do carry different addresses
//  there, so the shipped code's `jmp [eax+184h]` is a real dispatch.
//----------------------------------------------------------------------------
typedef void (__thiscall *Slot0x184Fn)(void* self);

void CSelectable::Deactivate()
{
    CWidget::Deactivate();

    if (m_attachmentSlot1 != (unsigned int)-1)
        AttachmentRelease(m_attachmentSlot1);
    if (m_attachmentSlot2 != (unsigned int)-1)
        AttachmentRelease(m_attachmentSlot2);

    ((Slot0x184Fn)(*(void***)this)[0x184 / 4])(this);
}

//----------------------------------------------------------------------------
//  0x6F2C74C0 - slot 66.  A `call` and a `retn` rather than a tail `jmp`:
//  the callee cleans the one dword this function pushed, and this function
//  itself has none of its own to clean.
//----------------------------------------------------------------------------
float CSelectable::Method_0x108()
{
    return GetConfigFloatHW("SelectionCircle", "ScaleFactor", 0);
}

//----------------------------------------------------------------------------
//  0x6F2C7440 - slot 97.  The clear happens whether or not there was
//  anything to release, which is why the store sits outside the `if` - the
//  same shape CWar3Image::Deactivate has (widget_releaseslots.cpp).
//----------------------------------------------------------------------------
void CSelectable::ReleaseSelectionVisual()
{
    if (m_pSelectionVisual != 0)
        ReleaseSelectionVisualObject(m_pSelectionVisual);
    m_pSelectionVisual = 0;
}

//----------------------------------------------------------------------------
//  0x6F266CD0 / 0x6F266CE0 - slots 98 and 99.
//----------------------------------------------------------------------------
int CSelectable::Method_0x188()
{
    return 0;
}

int CSelectable::Method_0x18C()
{
    return 0;
}

//----------------------------------------------------------------------------
//  0x6F2C7750 - slot 103.
//----------------------------------------------------------------------------
int CSelectable::Method_0x19C()
{
    return SelectableSlot0x19CGate(this);
}

//----------------------------------------------------------------------------
//  0x6F2C76D0 - slot 105.  The doubled scale is written to *both* halves of
//  a two-float pair and the pair's address is handed to each attachment -
//  `fst`/`fstp` to two adjacent slots, then `lea edx,[esp+..]` on the lower
//  of the two.  A single float passed by address would need only one store.
//
//  `fadd st, st` is `x + x`; `x * 2.0f` loads the constant and `fmul`s.
//----------------------------------------------------------------------------
void CSelectable::SetAttachmentScale(float scale)
{
    float doubled = scale + scale;
    float scaleXY[2];
    scaleXY[0] = doubled;
    scaleXY[1] = doubled;

    if (m_attachmentSlot1 != (unsigned int)-1)
        AttachmentSetScale(m_attachmentSlot1, scaleXY);
    if (m_attachmentSlot2 != (unsigned int)-1)
        AttachmentSetScale(m_attachmentSlot2, scaleXY);
}

//----------------------------------------------------------------------------
//  0x6F2C7720 - slot 106.  Second call is a tail jump.
//----------------------------------------------------------------------------
void CSelectable::ShowAttachments()
{
    if (m_attachmentSlot1 != (unsigned int)-1)
        AttachmentSetVisible(m_attachmentSlot1, 1);
    if (m_attachmentSlot2 != (unsigned int)-1)
        AttachmentSetVisible(m_attachmentSlot2, 1);
}
