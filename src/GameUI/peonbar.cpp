//============================================================================
//  0x6F36A9D0 - CPeonBar::CPeonBar.
//
//  The idle-worker button in the corner of the screen.  The class name is the
//  shipped one - ??_7CPeonBar@@6B@ at +0x00 and its `_0` twin at +0x124 - and
//  so is its module: the allocation it makes is tagged
//  "e:\\Drive1\\temp\\buildwar3x\\War3\\Source\\UI/CPeonBar.cpp".
//
//  The two vtables at +0x00 and +0x124 are the same shape CInfoPanelGroup
//  has: a CSimpleFrame (exactly 0x124 bytes) followed by a CObserver, so the
//  bar's own fields start at +0x130.
//
//  What it builds is one 0x1B8-byte CCommandButton - which is
//  CCommandButton's own size to the byte, and the second confirmation of that
//  class's layout - filling the bar, hidden to start with, listening through
//  the bar's own CObserver base, carrying the two IDLE_PEON game strings as
//  its tooltip and skinned from the "IdlePeon" skin key.
//
//  Two details are the shipped body's:
//
//    * `this` is handed to SetAllPoints and to the flag setter with no
//      adjustment at all - a CSimpleFrame is a CLayoutFrame at offset zero,
//      and the observer argument is the +0x124 base taken by address - which
//      is what the bare `push esi` and `push ebp` say;
//    * the button is shown-flag-cleared and then dispatched through slot
//      0x64, which is the other half of the 0x64/0x68 pair
//      GameUI/gameuiframehide.cpp names from CSimpleFrame::Hide's side.
//
//  Score capped by the __except_handler4 frame the shipped build puts round a
//  constructor with two bases and an owned allocation to unwind, *plus* the
//  two /GS cookie checks its 0x110 bytes of string buffers earn it - this
//  repo's fixed /GS- /EHs-c- emits neither.
//
//  Own translation unit: nine real calls out of it.
//============================================================================
#include "commandbutton.h"
#include "gameui.h"
#include "observer.h"
#include "gamestrings.h"
#include "storm.h"

//  0x6F603040 - Misc/sweep_field_setters.cpp, under the name that file gives
//  it.  Declared here on its own class rather than re-thunked: one shipped
//  address, one C++ name.
struct SDirtyFlagOwner
{
    void __thiscall SetField124And128(int a, int b);
};

//  0x6F3228E0 - GameUI/frameartpath.cpp, under the name that file gives it.
void __fastcall ApplyArtPathUnlessModel(void* frame, const char* path);

//  Slot 0x64 (+0x19) - the frame's own show/refresh, the other half of the
//  pair CSimpleFrame::Hide dispatches on.
typedef void (__thiscall *FrameSlot0x64Fn)(void* self);

static const char kPeonBarFile[] =
    "e:\\Drive1\\temp\\buildwar3x\\War3\\Source\\UI/CPeonBar.cpp";

class CPeonBar : public CSimpleFrame, public CObserver
{
public:
    CPeonBar(void* parent);         // 0x6F36A9D0, `retn 4`

    int             m_field130;     // +0x130
    CCommandButton* m_pButton;      // +0x134
    //  +0x138 .. +0x144 - five words the constructor clears.  The frame's
    //  own unwind trampoline destroys something at +0x138 through
    //  0x6F28D4E0, so at least the first of them is a sub-object rather
    //  than a plain field; the shipped constructor writes it as five zero
    //  stores either way.
    int             m_field138;     // +0x138
    int             m_field13C;     // +0x13C
    int             m_field140;     // +0x140
    int             m_field144;     // +0x144
    int             m_field148;     // +0x148
    float           m_float14C;     // +0x14C
    int             m_field150;     // +0x150
};

CPeonBar::CPeonBar(void* parent)
    : CSimpleFrame(parent)
{
    m_field130 = 0;
    m_field138 = 0;
    m_field13C = 0;
    m_field140 = 0;
    m_field144 = 0;
    m_float14C = 0.0f;
    m_field150 = 1;
    m_field148 = 0;

    SetWidth(0.038f);
    SetHeight(0.038f);

    void* memory = SMemAlloc(0x1B8, kPeonBarFile, 0x27, 0);
    m_pButton = memory ? new (memory) CCommandButton(this, 1) : 0;

    m_pButton->SetAllPoints(this, 1);

    //  The slot is named before the flag is stored, for the same reason
    //  CSimpleFrame::Hide names its own first.
    FrameSlot0x64Fn show = (FrameSlot0x64Fn)(*(void***)m_pButton)[0x64 / 4];
    m_pButton->m_shown = 0;
    show(m_pButton);

    ((SDirtyFlagOwner*)m_pButton)->SetField124And128(0, (int)(CObserver*)this);

    char title[0x80];
    char body[0x80];
    GetGameString("IDLE_PEON", title, sizeof(title));
    GetGameString("IDLE_PEON_DESC", body, sizeof(body));

    ((CUIToolTipFrame*)m_pButton)->SetToolTipText(title, body, 0, 0, 0);

    ApplyArtPathUnlessModel(m_pButton, ResolveSkinValue("IdlePeon", 0));
}
