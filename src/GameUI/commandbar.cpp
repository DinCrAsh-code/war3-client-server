//============================================================================
//  0x6F376550 - CCommandBar::CCommandBar: the command card.
//
//  The class name is the shipped one (??_7CCommandBar@@6B@, and only one
//  vtable - unlike the hero bar there is no CObserver base) and so is its
//  module, ".../UI/CCommandBar.cpp".
//
//  A 4 x 3 CSimpleGrid of twelve plain CCommandButtons, 0.1745 by 0.129,
//  anchored point 8 to point 8 of its parent at (0.002, -0.007375).  Ahead of
//  the grid it builds its own two registries - the 'command' table keyed on
//  an eight-character code and the 'order type' table keyed on a four-
//  character one, both TSHashTables and both still redirects
//  (Containers/hashtablectors.h) - and behind it, on a replay, it creates the
//  "SimpleReplayPanel" frame by name.
//
//  The loop order is the shipped one: the *column* is the outer index and the
//  row the inner, which is what `cmp ebp, 3` inside `cmp eax, 4` says, and it
//  is also the argument order SetCellFrame takes.
//
//  The list at +0x1B4 is a TSExplicitList with link offset 0, spelled out as
//  its four stores rather than as a member: the shipped constructor builds it
//  *after* the two table constructors, and a member of that type would be
//  built before the first statement of the body.
//
//  Score capped by the __except_handler4 frame the shipped build puts round a
//  constructor with a base, two sub-objects and a loop that allocates.
//
//  Own translation unit: eleven real calls out of it.
//============================================================================
#include "commandbutton.h"
#include "hashtablectors.h"
#include "framereg.h"
#include "storm.h"

//  0x6F53F160 - Game/gamemode.cpp, under the name that file gives it.
int __fastcall IsGameModeOne();

//  Slot 0x64 (+0x19) - the frame's own show/refresh.  Nothing pushed, which
//  is what the shipped call site says.
typedef void (__thiscall *FrameSlot0x64Fn)(void* self);

static const char kCommandBarFile[] =
    "e:\\Drive1\\temp\\buildwar3x\\War3\\Source\\UI/CCommandBar.cpp";

class CCommandBar : public CSimpleGrid
{
public:
    CCommandBar(void* parent);      // 0x6F376550, `retn 4`

    //  +0x15C - the replay panel, created only in a replay.
    void*              m_pReplayPanel;   // +0x15C
    SCommandRegTable   m_commands;       // +0x160 .. +0x188
    SOrderTypeRegTable m_orderTypes;     // +0x188 .. +0x1B0
    int                m_field1B0;       // +0x1B0
    //  +0x1B4 .. +0x1C0 - a TSExplicitList with link offset 0, written out.
    int                m_listLinkOffset; // +0x1B4
    void*              m_listNext;       // +0x1B8
    int                m_listPrevLink;   // +0x1BC
};

CCommandBar::CCommandBar(void* parent)
    : CSimpleGrid(parent)
{
    m_pReplayPanel = 0;

    m_commands.Construct();
    m_orderTypes.Construct();

    m_field1B0 = 1;

    m_listPrevLink   = 0;
    m_listNext       = (void*)&m_listNext;
    m_listLinkOffset = 0;
    m_listPrevLink   = ~(int)&m_listNext;

    SetSize(3, 4);

    SetWidth(0.1745f);
    SetHeight(0.129f);

    SetPointTo(8, (CLayoutFrame*)parent, 8, 0.002f, -0.007375f, 1);

    for (unsigned int column = 0; column < 4; column++)
    {
        for (unsigned int row = 0; row < 3; row++)
        {
            void* memory = SMemAlloc(0x1B8, kCommandBarFile, 0x16C, 0);
            CCommandButton* button =
                memory ? new (memory) CCommandButton(this, 0) : 0;

            FrameSlot0x64Fn show =
                (FrameSlot0x64Fn)(*(void***)button)[0x64 / 4];
            button->m_shown = 0;
            show(button);

            SetCellFrame(column, row, button, 1);
        }
    }

    if (IsGameModeOne())
        m_pReplayPanel = CreateSimpleFrameByName("SimpleReplayPanel", this, 0);
}
