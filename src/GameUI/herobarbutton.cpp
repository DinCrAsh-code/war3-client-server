//============================================================================
//  0x6F368F90 - CHeroBarButton::CHeroBarButton.
//
//  One command button with two stat bars stacked under it and a
//  "HeroBarPointModel" sprite over it.
//
//  The two stat bars are 0x158 bytes each, tagged with the bar's own module
//  and lines 0x39 and 0x3F, and they differ in one argument - 3 and 4 - which
//  is the only thing in this tree that separates them.  Each is 0.038 wide by
//  0.003125 high; the first hangs point 1 off point 7 of the button, the
//  second off point 7 of the first, both at (0, -0.00125).
//
//  The sprite frame is *not* parented on the button: its constructor is
//  handed the game UI singleton's own +0x424, which is what the shipped
//  `call GetGameUI` / `mov edi, [edi+424h]` pair says.  Its path comes from
//  the "HeroBarPointModel" skin key and goes in through vtable slot 0xE8
//  rather than by name - a hand-written dispatch, judged here only by its
//  own call site (0x6F3691A4 pushes exactly three dwords), because the
//  receiver is not `this` and there is no committed vtable for CSpriteFrame.
//  Slot 0xE8 is CSpriteFrame::SetSpritePath's own slot; the shipped code
//  reaches it indirectly and this file does the same rather than calling the
//  0x6F60F360 body by name, because a direct call is a different
//  instruction.
//
//  `this` goes into both SetPointTo calls and into SetAllPoints with no
//  adjustment and no null test: a CCommandButton is a CLayoutFrame at offset
//  zero, and `this` is never null.
//
//  Score capped by the __except_handler4 frame the shipped build puts round a
//  constructor with two bases, four sub-objects and three owned allocations
//  to unwind.
//
//  Own translation unit: twelve real calls out of it.
//============================================================================
#include "herobar.h"
#include "gameui.h"
#include "agenttypedslots.h"
#include "frame.h"

//  Slot 0xE8 (+0x3A) of CSpriteFrame - take a sprite path.  The argument
//  count is the shipped call site's: three dwords pushed.
typedef void (__thiscall *SpriteFrameSetPathFn)(void* self, const char* path,
                                                int a, int b);

static const char kHeroBarFile[] =
    "e:\\Drive1\\temp\\buildwar3x\\War3\\Source\\UI/CHeroBar.cpp";

//  Every member the shipped constructor sets before it starts allocating is
//  a *member initialiser* and not a body statement, and the list below is in
//  declaration order on purpose: that is the only spelling that puts the
//  '+w3u' slot's constructor, then four stores, then CEventTimer's
//  constructor, with no placement-new null test in front of either - which is
//  exactly the shipped stream.
CHeroBarButton::CHeroBarButton(void* parent)
    : CCommandButton(parent, 1),
      m_unit(0),
      m_field1C8(0),
      m_field1D4(0),
      m_field1DC(0),
      m_float1E0(0.0f),
      m_timer()
{
    void* barA = SMemAlloc(0x158, kHeroBarFile, 0x39, 0);
    m_pStatBarA = barA ? ((CStatBar*)barA)->Construct(this, 0, 3) : 0;

    ((CLayoutFrame*)m_pStatBarA)->SetWidth(0.038f);
    ((CLayoutFrame*)m_pStatBarA)->SetHeight(0.003125f);
    ((CLayoutFrame*)m_pStatBarA)->SetPointTo(1, this, 7, 0.0f, -0.00125f, 1);

    void* barB = SMemAlloc(0x158, kHeroBarFile, 0x3F, 0);
    m_pStatBarB = barB ? ((CStatBar*)barB)->Construct(this, 0, 4) : 0;

    ((CLayoutFrame*)m_pStatBarB)->SetWidth(0.038f);
    ((CLayoutFrame*)m_pStatBarB)->SetHeight(0.003125f);
    ((CLayoutFrame*)m_pStatBarB)->SetPointTo(1, (CLayoutFrame*)m_pStatBarA, 7,
                                             0.0f, -0.00125f, 1);

    CGameUI* ui = (CGameUI*)GetGameUI(1, 0);

    void* sprite = SMemAlloc(0x1B0, kHeroBarFile, 0x47, 0);
    m_pPointSprite = sprite
        ? new (sprite) CSpriteFrame((int)ui->m_pSpriteHost, 0, 0)
        : 0;

    SpriteFrameSetPathFn setPath =
        (SpriteFrameSetPathFn)(*(void***)m_pPointSprite)[0xE8 / 4];
    setPath(m_pPointSprite, ResolveSkinValue("HeroBarPointModel", 0), -1, 0);

    m_pPointSprite->SetAllPoints(this, 1);

    m_pPointSprite->m_flags |= m_pPointSprite->m_flagMask | 2;

    m_field1DC = 0xFFFF4444;
}
