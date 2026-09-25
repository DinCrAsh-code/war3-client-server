//============================================================================
//  0x6F26E350 - CUnit's own override of slot 61 (RefreshSpriteVisibility),
//  CWidget's own version being widget_slot61.cpp's CWidget::RefreshSpriteVisibility.
//
//  Same three ingredients as the base version - the branchless
//  "(m_reserved24 & 0x80000000) != 0" visible flag, a visibility predicate,
//  and two CWar3Image::SetSpriteVisible calls - but CUnit's predicate is
//  CUnit::QueryVisibleImpl(1) gated by CWidget::IsVisibleToLocalPlayer()
//  rather than the base's own IsGameModeOne()/world-record tail, and this
//  override goes on afterwards to push the same "shown" state at the
//  unit's own model instance through two more probes
//  (spritesethidden.cpp's 0x6F4D3500/0x6F4D3510).
//
//  Own translation unit: reaches CWidget::IsVisibleToLocalPlayer,
//  CUnit::QueryVisibleImpl, CWar3Image::SetSpriteVisible and the two model
//  probes below all with real calls.
//============================================================================
#include "unit.h"

extern void __fastcall NotifyModelAttachmentFlag_6F4D3500(class CSprite*, int);
extern void __fastcall NotifyModelTreeFlag_6F4D3510(class CSprite*, int);

void CUnit::RefreshSpriteVisibility()
{
    //  Branchless visible flag, same trick as the base class's own version:
    //  `0u < (m_reserved24 & 0x80000000u)` rather than `!= 0`, so MSVC folds
    //  it to the shipped `xor/cmp/sbb/neg` sequence instead of a
    //  compare-and-branch.
    int visible = (0u < (m_reserved24 & 0x80000000u));

    //  `shown`: already true if `visible` is, otherwise
    //  IsVisibleToLocalPlayer() && QueryVisibleImpl(1) - both real calls,
    //  and IsVisibleToLocalPlayer() is evaluated even when it will not be
    //  used (the shipped code calls it unconditionally up front, into
    //  `edi`, before the `visible` test that may skip QueryVisibleImpl
    //  altogether).
    int isVisibleToLocalPlayer = IsVisibleToLocalPlayer();
    int shown;
    if (visible != 0)
        shown = 1;
    else if (isVisibleToLocalPlayer == 0)
        shown = 0;
    else
        shown = (QueryVisibleImpl(1) != 0);

    CWar3Image::SetSpriteVisible(shown, visible);

    //  Second flag, independent of `shown`/`visible` above: true only when
    //  both IsVisibleToLocalPlayer() and the unit's own +0x114 word are
    //  zero.  `m_int114` is read once, at function entry, well before this
    //  point - the shipped code holds it live in `ebx` across both calls
    //  above rather than re-reading it here.
    int notifyFlag = (isVisibleToLocalPlayer == 0 && m_int114 == 0);

    NotifyModelAttachmentFlag_6F4D3500(m_pSprite, notifyFlag);

    //  Tail end: `NotifyModelTreeFlag_6F4D3510` gets
    //  `isVisibleToLocalPlayer == 0` as its own flag, not `notifyFlag`.
    NotifyModelTreeFlag_6F4D3510(m_pSprite, (isVisibleToLocalPlayer == 0));
}
