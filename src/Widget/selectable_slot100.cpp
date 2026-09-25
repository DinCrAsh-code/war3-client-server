//============================================================================
//  0x6F2C74E0 - CSelectable's vtable slot 100 (+0x190), inherited unchanged
//  by CItem.  CItem's slot 102 (item_selectionrefresh.cpp) calls it with
//  (0, 0) before walking the player selection records.
//
//  The selection *highlight*, as against slot 101's selection *circle*:
//  create the +0x48 attachment if the local player is allowed to see this
//  object's highlight at all, drive the selection visual, and then either
//  half-dim the highlight (alpha 0x80) and light it, or turn it off.
//
//  Every path is gated on IsGameModeOne() being false - whatever mode 1 is,
//  an object in it gets no highlight at all - which is the flag all three
//  blocks test.  With that flag known zero on the fall-through, MSVC pushes
//  the register holding it wherever the source has a literal 0: the shipped
//  `push ebx` triple in the creation block is those three zeros, not three
//  copies of the flag.
//
//  Own translation unit: seven real calls out of it, none to each other.
//============================================================================
#include "selectable.h"
#include "fvec3.h"
#include "stormnotifysingleton.h"   // NotifyStormSingleton - 0x6F00D990

//  0x6F53F160 - gamemode.cpp.  Already reconstructed and EXACT, and already
//  named for what it tests rather than for what that mode means;
//  re-declared here, never re-thunked - a second naked body at the same
//  address is the two-symbols-for-one-function defect
//  docs/notes/verifier-gate-link-failures.md warns about, and it cost this
//  function its own EXACT the first time this file was written.
int __fastcall IsGameModeOne();

//  0x6F00A8B0 / 0x6F00D9D0 - configcolor.cpp / stormsingleton_attach.cpp.
unsigned int* __fastcall GetConfigColor(unsigned int* out, const char* section,
                                        const char* key, int index);
void __fastcall SetVisualAttachmentColor(int index, const unsigned int* color);

//  0x6F300710 / 0x6F00DA90 - selectable_attachcreate.cpp declares and
//  redirects both; re-declared here rather than redirected a second time,
//  which would be two symbols for one address.
struct SGameUI
{
    char          m_reserved00[0x29C];
    unsigned int  m_highlightsOn;   // +0x29C
    unsigned int  m_uiUp;           // +0x2A0
    unsigned int  m_circlesLit;     // +0x2A4
};
SGameUI* __fastcall GetGameUI(int create, int reset);
int __fastcall CreateVisualAttachment(const FVec3* scale, const FVec3* pos, int flags);

//  slot 66 (+0x108) - the selection-circle radius, in st0.
typedef float (__thiscall *Slot0x108Fn)(void* self);
//  slot 56 (+0xE0) - CWidget::GetPosition.
typedef FVec3* (__thiscall *Slot0xE0Fn)(void* self, FVec3* out, int wantGround,
                                        int a3, int a4);
//  slot 98 (+0x188) - "am I the locally selected agent" (CItem::Method_0x188,
//  item_selected.cpp; CSelectable's own answer is a flat 0).
typedef int (__thiscall *Slot0x188Fn)(void* self);

void CSelectable::RefreshSelectionHighlight(int wantHighlight, int visualArg)
{
    int modeOne = IsGameModeOne();

    //  The AND, not a second `if`: the shipped code computes
    //  `arg & GetGameUI(1,0)->m_highlightsOn` once, up front and outside
    //  every branch, and tests the result three times further down.
    int showHighlight = wantHighlight & (int)GetGameUI(1, 0)->m_highlightsOn;

    if (!modeOne && showHighlight &&
        m_attachmentSlot2 == (unsigned int)-1)
    {
        float radius = ((Slot0x108Fn)(*(void***)this)[0x108 / 4])(this);

        FVec3 scale;
        scale.m_x = radius;
        scale.m_y = radius;
        scale.m_z = radius;

        FVec3 pos;
        ((Slot0xE0Fn)(*(void***)this)[0xE0 / 4])(this, &pos, 0, -1, 0);

        m_attachmentSlot2 = (unsigned int)CreateVisualAttachment(&scale, &pos, 0);

        unsigned int color;
        GetConfigColor(&color, "SelectionCircle", "ColorNeutral", 0);
        SetVisualAttachmentColor((int)m_attachmentSlot2, &color);
    }

    //  Both arguments or neither: the shipped code branches on `visualArg`
    //  alone and pushes (showHighlight, visualArg) or (0, 0), so a zero
    //  `visualArg` suppresses the highlight flag too.
    if (visualArg != 0)
        UpdateSelectionVisual(showHighlight, visualArg);
    else
        UpdateSelectionVisual(0, 0);

    if (modeOne)
        return;

    int selected = ((Slot0x188Fn)(*(void***)this)[0x188 / 4])(this);

    if (showHighlight)
    {
        //  Nothing to do when this object is already the selected one - it
        //  gets the full-strength treatment elsewhere.
        if (selected != 0)
            return;

        unsigned int color;
        GetConfigColor(&color, "SelectionCircle", "ColorNeutral", 0);

        //  Half alpha, written as a byte straight into the packed colour's
        //  top channel - the shipped `mov byte ptr [...+3], 80h`.
        ((unsigned char*)&color)[3] = 0x80;

        SetVisualAttachmentColor((int)m_attachmentSlot2, &color);
        NotifyStormSingleton((int)m_attachmentSlot2, 1);
        return;
    }

    if (m_attachmentSlot2 != (unsigned int)-1)
        NotifyStormSingleton((int)m_attachmentSlot2, 0);
}
