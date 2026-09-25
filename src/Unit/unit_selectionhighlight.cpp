//============================================================================
//  0x6F285770 - CUnit's own override of vtable slot 100 (+0x190), which
//  CSelectable declares (selectable_slot100.cpp) and CItem inherits
//  unchanged.
//
//  Calls straight into the base body with the already-masked
//  `wantHighlight & GameUI->m_highlightsOn` value (harmless double-AND
//  once inside the base, since ANDing twice with the same mask is a
//  no-op) so the masked value survives the call for this override's own
//  extra work:
//
//    * when the mask comes out false, clear bit 0x100 of `m_flags280`
//      (slot 56's own "hand bit 1 out" flag word, unit.h);
//    * skip everything else in game mode 1 (IsGameModeOne);
//    * the first time this unit gets an attachment slot at all
//      (`m_attachmentSlot2 == g_unk6F932954`, the shared "none" sentinel
//      every attachment field in this class compares against) *and* the
//      mask is true, push a one-time Z offset - the footprint type's own
//      "zOffset" property minus `m_bound210`'s hardware-float bit
//      pattern - onto the freshly created attachment, then latch
//      `m_attachmentRefreshFlag`;
//    * when the mask is true at all, pick this unit's selection-circle
//      colour from `GetPlayerRelationConstant(localSlot, ownerSlot)`
//      against config's own "ColorFriend"/"ColorNeutral" (the two
//      candidates RefreshSelectionHighlight already uses, so a match
//      keeps the relation colour and a miss falls back to
//      "ColorNeutral"), half-dims it (alpha 0x80, the base's own trick),
//      and pushes it onto the attachment.
//
//  `m_bound210` is declared `CFloat` (unit.h - "the FloatMiniB's m_max")
//  but the shipped code reads its bit pattern straight into the FPU with
//  a plain `fsub`, not through the software-float path: the same
//  `CFloat` vs `float` return-convention split docs/msvc-vc8-idioms.md
//  documents for GetDefaultConfigFloat, here for a field read instead of
//  a return value.
//
//  Own translation unit: seven real calls out of it (GetGameUI,
//  RefreshSelectionHighlight, IsGameModeOne, GetFootprintZOffset,
//  SetVisualAttachmentZ, GetPlayerRelationConstant [virtual
//  GetOwningPlayerIndex feeds it], GetConfigColor, SetVisualAttachmentColor),
//  none to each other.
//============================================================================
#include "unit.h"
#include "../Agent/relationconstant.h"
#include "../Pathfinding/footprinttype.h"

//  0x6F300710 - selectable_slot100.cpp's own declaration.
struct SGameUI
{
    char          m_reserved00[0x29C];
    unsigned int  m_highlightsOn;   // +0x29C
};
SGameUI* __fastcall GetGameUI(int create, int reset);

//  0x6F53F160 - gamemode.cpp.  Already reconstructed and EXACT; re-declared
//  rather than re-thunked (docs/notes/verifier-gate-link-failures.md).
int __fastcall IsGameModeOne();

//  0x6F00D9A0 / 0x6F00D9D0 - stormsingleton_attach.cpp.
void __fastcall SetVisualAttachmentZ(int index, int onWalkable, float z);
void __fastcall SetVisualAttachmentColor(int index, const unsigned int* color);

//  0x6F00A8B0 - Config/configcolor.cpp.
unsigned int* __fastcall GetConfigColor(unsigned int* out, const char* section,
                                        const char* key, int index);

//  the "none" sentinel every attachment-slot field is compared against
//  (unit_attachmentplacement.cpp).
extern const unsigned int g_unk6F932954;

//  the player-slot table singleton's own "acting (local) player" slot -
//  worldframelocalplayer.cpp's own view, re-declared locally per this
//  repo's own rule for IndexedArrayHolder_6F3A1650 (each caller of the
//  underlying table declares only the fields it touches).
extern void* g_unk6FAB65F4;
struct SActingPlayerSlotHolder
{
    char           m_reserved00[0x28];
    unsigned short m_actingSlot;   // +0x28
};

void CUnit::RefreshSelectionHighlight(int wantHighlight, int visualArg)
{
    //  Read ahead of the mask/base-call pair below, and kept in a
    //  callee-saved register across both: the shipped body computes this
    //  before it even calls GetGameUI, not beside the field it gates.
    int noAttachmentYet = (m_attachmentSlot2 == g_unk6F932954);

    int showHighlight = wantHighlight & (int)GetGameUI(1, 0)->m_highlightsOn;

    CSelectable::RefreshSelectionHighlight(showHighlight, visualArg);

    if (!showHighlight)
        m_flags280 &= ~0x100;

    if (IsGameModeOne())
        return;

    if (noAttachmentYet && showHighlight)
    {
        float zOffset = GetFootprintZOffset(m_footprintType);
        if (zOffset > 0.0f)
        {
            SetVisualAttachmentZ((int)m_attachmentSlot2, 1,
                zOffset - *reinterpret_cast<float*>(&m_bound210.m_bits));
            m_attachmentRefreshFlag = 1;
        }
    }

    if (!showHighlight)
        return;

    int localSlot = ((SActingPlayerSlotHolder*)g_unk6FAB65F4)->m_actingSlot;
    //  Zero-initialised ahead of the two calls that ultimately fill it in -
    //  the shipped body writes this slot's zero before its own call to
    //  GetOwningPlayerIndex(), not merely before the read that matters.
    unsigned int color = 0;

    color = GetPlayerRelationConstant(localSlot, GetOwningPlayerIndex()).m_value;

    if (m_int60 & 1)
    {
        unsigned int friendColor;
        GetConfigColor(&friendColor, "SelectionCircle", "ColorFriend", 0);
        if (color != friendColor)
        {
            unsigned int neutralColor;
            GetConfigColor(&neutralColor, "SelectionCircle", "ColorNeutral", 0);
            color = neutralColor;
        }
    }

    ((unsigned char*)&color)[3] = 0x80;
    SetVisualAttachmentColor((int)m_attachmentSlot2, &color);
}
