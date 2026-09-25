//============================================================================
//  0x6F3692D0 - CHeroBar::CHeroBar.
//
//  A one-column, seven-row CSimpleGrid of CHeroBarButtons.  0.038 wide by
//  0.35825 high - seven buttons of 0.038 plus the gaps - and each cell is one
//  0x204-byte button, hidden, given the flag 0x50 and the bar's own CObserver
//  base to report to.
//
//  The bar hides *itself* at the end when the "show hero bar" preference
//  (key 0x2E) reads zero: the shipped body calls GetPreferences, asks for the
//  value into a local, and only then tests it, which is why the local is
//  written and read rather than the call's own return value being tested.
//
//  The list at +0x168 is a TSExplicitList with link offset 0 - the
//  constructor writes 0 into +0x168 and self-links the terminator at +0x16C -
//  and it goes in the member-initialiser list ahead of +0x17C and +0x180,
//  which is the shipped order.
//
//  Score capped by the __except_handler4 frame the shipped build puts round a
//  constructor with two bases and a loop that allocates.
//
//  Own translation unit: seven real calls out of it, and its own button's
//  constructor is a real call into a neighbouring module.
//============================================================================
#include "herobar.h"
#include "frame.h"
#include "preferences.h"
#include "tslist.inl"

//  0x6F603030 / 0x6F603040 - Misc/sweep_field_setters.cpp, under the names
//  that file gives them.
struct SDirtyFlagOwner
{
    void __thiscall SetField140(int value);
    void __thiscall SetField124And128(int a, int b);
};

//  Slot 0x64 (+0x19) - the frame's own show/refresh, the other half of the
//  pair CSimpleFrame::Hide dispatches on.  The argument count is the shipped
//  call site's: nothing pushed.
typedef void (__thiscall *FrameSlot0x64Fn)(void* self);

static const char kHeroBarFile[] =
    "e:\\Drive1\\temp\\buildwar3x\\War3\\Source\\UI/CHeroBar.cpp";

CHeroBar::CHeroBar(void* parent)
    : CSimpleGrid(parent),
      m_list(),
      m_field17C(1),
      m_field180(3)
{
    SetSize(7, 1);

    SetWidth(0.038f);
    SetHeight(0.35825f);

    for (int row = 0; row < 7; row++)
    {
        void* memory = SMemAlloc(0x204, kHeroBarFile, 0xDC, 0);
        CHeroBarButton* button =
            memory ? new (memory) CHeroBarButton(this) : 0;

        //  The slot is named before the flag is stored, for the same reason
        //  CSimpleFrame::Hide names its own first.
        FrameSlot0x64Fn show = (FrameSlot0x64Fn)(*(void***)button)[0x64 / 4];
        button->m_shown = 0;
        show(button);

        ((SDirtyFlagOwner*)button)->SetField140(0x50);
        ((SDirtyFlagOwner*)button)->SetField124And128(0, (int)(CObserver*)this);

        SetCellFrame(0, row, button, 1);
    }

    m_float178 = 0.0f;
    m_field174 = 0;

    int show = 0;
    GetPreferences()->GetIntValue(0x2E, &show);

    if (show == 0)
    {
        FrameSlot0x64Fn refresh = (FrameSlot0x64Fn)(*(void***)this)[0x64 / 4];
        m_shown = 0;
        refresh(this);
    }
}

//  The bar's own list at +0x168 is a TSExplicitList, so ~CHeroBar reaches
//  TSExplicitList::UnlinkAll - which is out of line by construction
//  (Containers/tslist.inl says why).  Instantiated here so the .mix links;
//  the element type is a placeholder, because nothing in this call tree ever
//  puts a node into this list.
template void TSExplicitList<void, 0>::UnlinkAll();
