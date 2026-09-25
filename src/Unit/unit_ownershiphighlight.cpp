//============================================================================
//  0x6F29D880 - CUnit's own vtable slot 45 (+0xB4).  `this` in ecx, no
//  stack arguments, `retn 0`.
//
//  Picks the sprite's team-colour tint for this frame and pushes it,
//  exactly like slot 65's RefreshTerrainTint (unit_terraintint.cpp) - but
//  the colour choice here is about *ownership state*, not terrain, and the
//  result also drives two model-tree notifications RefreshTerrainTint
//  never touches.
//
//  Colour choice, in the shipped order:
//    - the owning player's bit set in the world-record +0x2E0 mask
//      (IsOwningPlayerInWorldMask2E0, unit_ownerworldmask.cpp)
//                                                    -> 0xFF1460FF
//    - else m_int158 > 0                            -> 0xFFC0FF80
//    - else m_int150 != 0                           -> 0xFF80C0C0
//    - else m_int154 != 0                           -> 0xFFA04040
//    - else bit 0x800 or bit 0x400 of m_flags5C set  -> 0xFFFFFFFF
//    - else                                          -> m_int2D4 (the same
//      per-unit fallback colour RefreshTerrainTint reads)
//
//  When the unit is visible (QueryVisible(1), CUnit's own slot 60) that
//  colour is blended against the terrain sample the same way
//  RefreshTerrainTint blends its own fixed colour - SampleFootprintTerrainTint
//  through ModulateColors - before being pushed; when it is not visible the
//  raw colour goes straight to the sprite.
//
//  Two flags, both keyed off the *same* two booleans - whether the unit has
//  a positive m_int114 threshold (`hasThreshold`, m_int114 != 0 ||
//  m_int158 > 0) and whether the local player can see this unit's owner
//  (`ownerVisible`, IsVisibleToLocalPlayer, widget_visibleflags.cpp) -
//  drive the two model-tree notifications at the end: the attachment flag
//  is set only when *neither* is true, the tree flag whenever the
//  visibility check alone is false.  Both notifications are on
//  CUnit's own vtable-slot-46-adjacent sprite, not on `this`.
//============================================================================
#include "unit.h"
#include "sprite.h"
#include "packedcolor.h"

extern void __fastcall NotifyModelAttachmentFlag_6F4D3500(class CSprite*, int);
extern void __fastcall NotifyModelTreeFlag_6F4D3510(class CSprite*, int);

//  Named Method_0xB4, not something readable: C++ binds a virtual override
//  by name, and this has to stay the same name CWidget's own body at this
//  slot uses (CWidget::Method_0xB4, widget_vtable_leaves.cpp) or it stops
//  being an override at all - see unit.h's own note on the declaration.
int CUnit::Method_0xB4()
{
    //  Computed first, ahead of the whole colour choice below - the
    //  shipped body's very first call, matching the order
    //  IsVisibleToLocalPlayer's own result (ownerVisible) is used near the
    //  end.
    int ownerVisible = IsVisibleToLocalPlayer();

    int hasThreshold = (m_int114 != 0 || (int)m_int158 > 0) ? 1 : 0;

    unsigned int color;
    if (IsOwningPlayerInWorldMask2E0())
        color = 0xFF1460FFu;
    else if ((int)m_int158 > 0)
        color = 0xFFC0FF80u;
    else if (m_int150 != 0)
        color = 0xFF80C0C0u;
    else if (m_int154 != 0)
        color = 0xFFA04040u;
    else if (m_flags5C & 0x800)
        color = 0xFFFFFFFFu;
    else if (m_flags5C & 0x400)
        color = 0xFFFFFFFFu;
    else
        color = m_int2D4;

    if (QueryVisible(1))
    {
        unsigned int terrainTint;
        SampleFootprintTerrainTint(&terrainTint);

        SPackedColor blend;
        color = *(const unsigned int*)ModulateColors(
            &blend, (const SPackedColor*)&terrainTint,
            (const SPackedColor*)&color);
    }

    SetSpriteTeamColor(m_pSprite, 0, (int)color);

    if (!hasThreshold)
        SetSpriteSlot34(m_pSprite, ((const unsigned char*)&color)[3], 0);

    NotifyModelAttachmentFlag_6F4D3500(m_pSprite,
                                       (!hasThreshold && !ownerVisible) ? 1 : 0);
    NotifyModelTreeFlag_6F4D3510(m_pSprite, ownerVisible == 0);
    return 0;
}
