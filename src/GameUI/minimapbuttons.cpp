//============================================================================
//  0x6F2F6040 - CGameUI::CreateMiniMapButtons.
//
//  The column of five buttons down the right-hand side of the minimap: ping,
//  and the four toggles the four RefreshMiniMap*ButtonSkin bodies re-skin.
//  Every allocation is tagged ".\\CGameUI.cpp", so this is CGameUI's own
//  module, and the six line numbers (0x2C9, 0x2CC, 0x2DC, 0x2E9, 0x2F6,
//  0x303) are the shipped ones.
//
//  A bare 0x124-byte CSimpleFrame goes in at +0x3D4 first and every button is
//  parented on it; each button is a 0x168-byte CSimpleButton, told its own
//  command id (0x15, 0x16, 0x17, 0x18, 0x1B), flagged 0x50, sized
//  0.023125 x 0.020625, anchored point 0 to point 6 of the frame at +0x428 -
//  at x 0.154375 for the first four and 0.155 for the last - and given the
//  same (0x19, 0x1A) pair.  Only the first of the five has its own skins set
//  here; the other four get theirs from the refreshers at the end.
//
//  On a replay the formation toggle and the ping button are disabled.
//
//  The five bodies are written out rather than looped: the shipped stream has
//  them one after another with a different line number, id and y in each, and
//  a loop over a table would be a different function.
//
//  Score capped by the __except_handler4 frame the shipped build puts round
//  six owned allocations.
//
//  Own translation unit: thirty-eight real calls out of it.
//============================================================================
#include "gameui.h"
#include "storm.h"

//  0x6F603030 / 0x6F603040 / 0x6F603060 - Misc/sweep_field_setters.cpp, under
//  the names that file gives them.
struct SDirtyFlagOwner
{
    void __thiscall SetField140(int value);
    void __thiscall SetField124And128(int a, int b);
    void __thiscall SetField12CTriple(int a, int b, int c);
};

//  0x6F53F160 - Game/gamemode.cpp.
int __fastcall IsGameModeOne();

static const char kGameUIFile[] = ".\\CGameUI.cpp";

void CGameUI::CreateMiniMapButtons()
{
    void* host = SMemAlloc(0x124, kGameUIFile, 0x2C9, 0);
    m_pMiniMapButtonHost =
        host ? new (host) CSimpleFrame((void*)m_unitTipStyle) : 0;

    void* signal = SMemAlloc(0x168, kGameUIFile, 0x2CC, 0);
    m_pMiniMapSignalButton =
        signal ? new (signal) CSimpleButton(m_pMiniMapButtonHost) : 0;

    ((SDirtyFlagOwner*)m_pMiniMapSignalButton)->SetField124And128(0x15,
                                                                 (int)this);
    ((SDirtyFlagOwner*)m_pMiniMapSignalButton)->SetField140(0x50);
    m_pMiniMapSignalButton->SetHeight(0.020625f);
    m_pMiniMapSignalButton->SetWidth(0.023125f);
    m_pMiniMapSignalButton->SetPointTo(0, (CLayoutFrame*)m_unitTipStyle, 6,
                                       0.154375f, 0.14375f, 1);

    m_pMiniMapSignalButton->SetSlotTexture(
        1, ResolveSkinValue("MiniMapSignalButtonEnabled", 0));
    m_pMiniMapSignalButton->SetSlotTexture(
        2, ResolveSkinValue("MiniMapSignalButtonPushed", 0));
    m_pMiniMapSignalButton->SetSlotTexture(
        0, ResolveSkinValue("MiniMapSignalButtonDisabled", 0));

    ((SDirtyFlagOwner*)m_pMiniMapSignalButton)->SetField12CTriple(0x19, 0x1A,
                                                                 (int)this);

    void* terrain = SMemAlloc(0x168, kGameUIFile, 0x2DC, 0);
    m_pMiniMapTerrainButton =
        terrain ? new (terrain) CSimpleButton(m_pMiniMapButtonHost) : 0;

    ((SDirtyFlagOwner*)m_pMiniMapTerrainButton)->SetField124And128(0x16,
                                                                  (int)this);
    ((SDirtyFlagOwner*)m_pMiniMapTerrainButton)->SetField140(0x50);
    m_pMiniMapTerrainButton->SetHeight(0.020625f);
    m_pMiniMapTerrainButton->SetWidth(0.023125f);
    m_pMiniMapTerrainButton->SetPointTo(0, (CLayoutFrame*)m_unitTipStyle, 6,
                                        0.154375f, 0.121875f, 1);
    ((SDirtyFlagOwner*)m_pMiniMapTerrainButton)->SetField12CTriple(0x19, 0x1A,
                                                                  (int)this);

    void* ally = SMemAlloc(0x168, kGameUIFile, 0x2E9, 0);
    m_pMiniMapAllyButton =
        ally ? new (ally) CSimpleButton(m_pMiniMapButtonHost) : 0;

    ((SDirtyFlagOwner*)m_pMiniMapAllyButton)->SetField124And128(0x17,
                                                               (int)this);
    ((SDirtyFlagOwner*)m_pMiniMapAllyButton)->SetField140(0x50);
    m_pMiniMapAllyButton->SetHeight(0.020625f);
    m_pMiniMapAllyButton->SetWidth(0.023125f);
    m_pMiniMapAllyButton->SetPointTo(0, (CLayoutFrame*)m_unitTipStyle, 6,
                                     0.154375f, 0.1f, 1);
    ((SDirtyFlagOwner*)m_pMiniMapAllyButton)->SetField12CTriple(0x19, 0x1A,
                                                               (int)this);

    void* creep = SMemAlloc(0x168, kGameUIFile, 0x2F6, 0);
    m_pMiniMapCreepButton =
        creep ? new (creep) CSimpleButton(m_pMiniMapButtonHost) : 0;

    ((SDirtyFlagOwner*)m_pMiniMapCreepButton)->SetField124And128(0x18,
                                                                (int)this);
    ((SDirtyFlagOwner*)m_pMiniMapCreepButton)->SetField140(0x50);
    m_pMiniMapCreepButton->SetHeight(0.020625f);
    m_pMiniMapCreepButton->SetWidth(0.023125f);
    m_pMiniMapCreepButton->SetPointTo(0, (CLayoutFrame*)m_unitTipStyle, 6,
                                      0.154375f, 0.0775f, 1);
    ((SDirtyFlagOwner*)m_pMiniMapCreepButton)->SetField12CTriple(0x19, 0x1A,
                                                                (int)this);

    void* formation = SMemAlloc(0x168, kGameUIFile, 0x303, 0);
    m_pMiniMapFormationButton =
        formation ? new (formation) CSimpleButton(m_pMiniMapButtonHost) : 0;

    ((SDirtyFlagOwner*)m_pMiniMapFormationButton)->SetField124And128(0x1B,
                                                                    (int)this);
    ((SDirtyFlagOwner*)m_pMiniMapFormationButton)->SetField140(0x50);
    m_pMiniMapFormationButton->SetHeight(0.020625f);
    m_pMiniMapFormationButton->SetWidth(0.023125f);
    m_pMiniMapFormationButton->SetPointTo(0, (CLayoutFrame*)m_unitTipStyle, 6,
                                          0.155f, 0.05f, 1);
    ((SDirtyFlagOwner*)m_pMiniMapFormationButton)->SetField12CTriple(
        0x19, 0x1A, (int)this);

    RefreshMiniMapFormationButtonSkin();
    RefreshMiniMapTerrainButtonSkin();
    RefreshMiniMapAllyButtonSkin();
    RefreshMiniMapCreepButtonSkin();

    if (IsGameModeOne())
    {
        m_pMiniMapFormationButton->SetEnabled(0);
        m_pMiniMapSignalButton->SetEnabled(0);
    }
}
