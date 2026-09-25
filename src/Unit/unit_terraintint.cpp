//============================================================================
//  0x6F29D990 - CUnit's own override of vtable slot 65 (+0x104), where
//  CWidget/CSelectable share 0x6F2AD860 (widget_slot65.cpp) and CItem
//  overrides it separately (0x6F2B9BE0, item_slot65.cpp).
//
//  Unlike either of those, this override does not compare the new tint
//  against what the sprite currently shows before deciding whether to push
//  it - it always re-derives the colour and always re-applies it, gated
//  only on QueryVisible(0) (slot 60).  And where CWidget/CItem modulate the
//  terrain tint against a fixed per-widget or per-item-type colour, CUnit
//  modulates it against m_int2D4 - a per-unit value slot 67
//  (Method_0x10C) hands out untouched, and the only field this body reads
//  that neither of the other two slot-65 overrides has any analogue for.
//
//  The terrain sample itself is unit_footprinttint.cpp's own
//  SampleFootprintTerrainTint (0x6F29D690) - see there for the corner
//  logic.
//============================================================================
#include "unit.h"
#include "sprite.h"
#include "packedcolor.h"

void CUnit::RefreshTerrainTint()
{
    if (!QueryVisible(0))
        return;

    //  A snapshot, not the live field: the shipped code reads m_int2D4
    //  before the call below and modulates against that copy, not against
    //  &m_int2D4 itself.
    unsigned int ownColor = m_int2D4;

    unsigned int terrainTint;
    SampleFootprintTerrainTint(&terrainTint);

    SPackedColor blend;
    unsigned int tint = *(const unsigned int*)ModulateColors(
        &blend, (const SPackedColor*)&terrainTint,
        (const SPackedColor*)&ownColor);

    SetSpriteTeamColor(m_pSprite, 0, (int)tint);
    SetSpriteSlot34(m_pSprite, ((const unsigned char*)&tint)[3], 0);
}
