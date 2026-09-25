//============================================================================
//  0x6F2B9BE0 - CItem's override of vtable slot 65 (+0x104), where CWidget
//  and CSelectable share 0x6F2AD860 (widget_slot65.cpp).
//
//  Everything CWidget's own version does, with one extra step: the terrain
//  tint under the item is modulated by the item *type*'s own tint before it
//  is compared against what the sprite is currently showing.  An item type
//  with no record in the table contributes all-ones, which is the identity
//  for a per-channel modulate, so an unregistered type behaves exactly like
//  the plain-widget path.
//
//  Own translation unit: five real calls out of it, none to each other.
//============================================================================
#include "item.h"
#include "sprite.h"
#include "fvec3.h"
#include "packedcolor.h"
#include "game.h"       // CPathRef::SmartPtrToWorldVec3, CWorldVec3

//  0x6F2B8FE0 - item_typetint.cpp.
unsigned int* __fastcall QueryItemTypeTint(unsigned int* out, unsigned int typeId);
//  0x6F00BF60 - stormsingleton_terrain.cpp.
unsigned int* __fastcall QueryTerrainTintAt(unsigned int* out, const FVec3* point);

//  slot 46 (+0xB8) - the handle-bearing sub-object; CItem answers
//  &m_position (item.cpp).  Reached through the vtable because the shipped
//  code dispatches it rather than calling the override directly.
typedef void* (__thiscall *GetHandleObjectFn)(void*);

void CItem::RefreshTerrainTint()
{
    //  First, and before the world position is even asked for: the shipped
    //  code looks the type tint up at the very top of the function.
    //  Declared up here and assigned below, rather than initialised at the
    //  point of use.  It does not move them in the frame - MSVC gives these
    //  three the low slots either way, where the shipped code puts the
    //  type-tint scratch - but it does line the three
    //  store/`fld`/`fstp` triples up in the shipped order: initialised
    //  where they are used, MSVC interleaves the store of one component
    //  with the `fstp` of the previous one.  kCFloatNoInit because the
    //  assignments below are the only writes; see CFloat.h.
    CFloat wx(kCFloatNoInit);
    CFloat wy(kCFloatNoInit);
    CFloat wz(kCFloatNoInit);

    unsigned int typeTint;
    QueryItemTypeTint(&typeTint, m_footprintType);

    //  One expression, not a named intermediate - see widget_slot65.cpp.
    CWorldVec3 worldPos =
        ((const CPathRef*)((GetHandleObjectFn)(*(void***)this)[0xB8 / 4])(this))
            ->SmartPtrToWorldVec3();

    wx = worldPos.m_x;
    wy = worldPos.m_y;
    wz = worldPos.m_z;

    FVec3 point;
    point.m_x = *(const float*)&wx;
    point.m_y = *(const float*)&wy;
    point.m_z = *(const float*)&wz;

    unsigned int terrainTint;
    QueryTerrainTintAt(&terrainTint, &point);

    //  Read back through ModulateColors' own return value rather than out
    //  of the buffer that was handed to it: the shipped code has
    //  `mov edi,[eax]` off the returned pointer, not off the scratch slot's
    //  own address.
    SPackedColor blend;
    unsigned int tint = *(const unsigned int*)ModulateColors(
        &blend, (const SPackedColor*)&terrainTint,
        (const SPackedColor*)&typeTint);

    unsigned int current = 0;
    QuerySpriteSlot50Word(m_pSprite, (int*)&current);
    ((unsigned char*)&current)[3] = (unsigned char)QuerySpriteSlot4C(m_pSprite);

    if (tint != current)
    {
        SetSpriteTeamColor(m_pSprite, 0, (int)tint);
        SetSpriteSlot34(m_pSprite, ((const unsigned char*)&tint)[3], 0);
    }
}
