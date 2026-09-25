//============================================================================
//  0x6F2AD860 - CWidget's vtable slot 65 (+0x104), shared with CSelectable
//  (CItem overrides it at 0x6F2B9BE0).  Slot 45 (+0xB4) is a bare tail jump
//  into it (widget.h).
//
//  Re-tint this widget's sprite from the terrain underneath it: ask slot 46
//  for the handle-bearing sub-object, turn it into a world point, look the
//  terrain tint up at that point, push it back at this widget's own terrain
//  record if it has one, and then - only if the tint actually changed -
//  hand it to the sprite as its team colour and its alpha to sprite slot
//  0x34.
//
//  The "only if it changed" test compares the whole packed dword against
//  what the sprite currently reports, with the sprite's own slot-0x4C
//  answer patched into the top byte first: the sprite stores the colour and
//  the alpha in two different places, so neither half alone is the current
//  value.
//
//  Own translation unit: five real calls out of it, none to each other.
//============================================================================
#include "widget.h"
#include "sprite.h"
#include "fvec3.h"
#include "game.h"       // CPathRef::SmartPtrToWorldVec3, CWorldVec3

//  0x6F00BF60 / 0x6F00F7D0 - stormsingleton_terrain.cpp.
unsigned int* __fastcall QueryTerrainTintAt(unsigned int* out, const FVec3* point);
void __fastcall StoreTerrainTint(int recordIndex, const unsigned int* tint);

//  slot 46 (+0xB8) - the handle-bearing sub-object, the same shape
//  widget.cpp's own GetHandleObjectFn has.
typedef void* (__thiscall *GetHandleObjectFn)(void*);

void CWidget::RefreshTerrainTint()
{
    CFloat wx(kCFloatNoInit);
    CFloat wy(kCFloatNoInit);
    CFloat wz(kCFloatNoInit);

    //  One expression, not a named intermediate: the shipped code pushes
    //  SmartPtrToWorldVec3's own return buffer before the vtable[0xB8]
    //  call that produces its `this`, which MSVC only does when nothing
    //  names the pointer in between - the same idiom, for the same reason,
    //  as jasssetitemposition.cpp's own.
    CWorldVec3 worldPos =
        ((const CPathRef*)((GetHandleObjectFn)(*(void***)this)[0xB8 / 4])(this))
            ->SmartPtrToWorldVec3();

    //  Each component copied out *as CFloat* into a scratch slot of its own
    //  and only then reinterpreted as a hardware float - three integer
    //  `mov`s followed by three `fld`/`fstp` pairs, exactly as shipped.
    //  Reinterpreting straight off worldPos lets MSVC `fld` out of the
    //  return buffer and loses the integer copies.
    //  Declared above the position query and assigned here, not
    //  initialised at the point of use: initialised where they are used,
    //  MSVC interleaves the store of one component with the `fstp` of the
    //  previous one, where the shipped code emits each
    //  store/`fld`/`fstp` triple whole.  Same spelling as CItem's own
    //  override (item_slot65.cpp).
    wx = worldPos.m_x;
    wy = worldPos.m_y;
    wz = worldPos.m_z;

    FVec3 point;
    point.m_x = *(const float*)&wx;
    point.m_y = *(const float*)&wy;
    point.m_z = *(const float*)&wz;

    unsigned int tint;
    QueryTerrainTintAt(&tint, &point);

    //  Signed: -1 is this field's "no terrain record" sentinel (widget.h),
    //  and the shipped test is `jl`, not `jz`.
    if ((int)m_reserved38 >= 0)
        StoreTerrainTint((int)m_reserved38, &tint);

    //  The zero really is in the source: slot 0x50 fills this in, but the
    //  shipped code still stores 0 into the slot first.
    unsigned int current = 0;
    QuerySpriteSlot50Word(m_pSprite, (int*)&current);
    ((unsigned char*)&current)[3] = (unsigned char)QuerySpriteSlot4C(m_pSprite);

    if (tint != current)
    {
        SetSpriteTeamColor(m_pSprite, 0, (int)tint);
        SetSpriteSlot34(m_pSprite, ((const unsigned char*)&tint)[3], 0);
    }
}
