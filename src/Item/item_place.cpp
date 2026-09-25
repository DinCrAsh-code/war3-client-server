//============================================================================
//  0x6F2B7C60 - CItem's vtable slot 107 (+0x1AC): place a brand new item in
//  the world from a placement descriptor.
//
//  Find a free spot for the requested position (writing the adjusted
//  coordinates back into the descriptor if one had to be found), sample the
//  ground height there and record it, build the sprite through
//  CWidget::InitSprite, claim the pathing footprint, create the shadow,
//  fold seven of the descriptor's own flags into m_flags, copy three of its
//  words into the Position record, and finish with the same pathing-flag
//  tail CItem's slot 16 has (item_slot16.cpp).
//
//  The search box is degenerate - the same point for both corners - so
//  FindFreeWorldPoint is being asked "is exactly here free, and if not,
//  where near here is", not to search a rectangle.
//
//  One of the seven flag folds is *inverted*: bit 0x1000 is set when
//  m_field5C is **zero**, which is the shipped `and eax,0FFFFF000h` /
//  `add eax,1000h` pair rather than the plain `and eax,<bit>` the other six
//  use.  The same field is then copied into the Position record, so it is a
//  value in its own right and not only a flag.
//
//  Own translation unit: nine real calls out of it, three of them virtual.
//============================================================================
#include "item.h"
#include "boxmath.h"
#include "pathfind.h"       // FindFreeWorldPoint

//============================================================================
//  DELIBERATE BEHAVIOUR CHANGE - uncomment to log every item as it is
//  placed in the world (address + the values it is initialised to).
//  Implementation and the full write-up are in src/feature_itemdebug.cpp.
//
//  The switch is here, in the .cpp, and not in a header on purpose:
//  tools/build.sh fingerprints only the compile flags, so a header edit
//  does not force the TUs that include it to rebuild and the define would
//  go stale in every file but the one that happened to be recompiled.
//
//  While it is commented out this file compiles to exactly what it did
//  before and keeps its 124/128 score; uncommenting it adds a real call
//  and will show up as extra instructions against the shipped body.
//============================================================================
//#define FEATURE_ITEM_CREATE_LOG

#ifdef FEATURE_ITEM_CREATE_LOG
//  Declared here rather than in a header - see the note above.
void FeatureItemDebug_OnItemPlaced(CItem* item, unsigned int footprintType,
                                   const SItemPlacementDescriptor* desc,
                                   int afterPlacement);
#endif

//  dword_6FAAE4F4 - the same fixed radius CItem::RefreshPlacementRadius clamps
//  against (item_radius.cpp).
extern const CFloat g_slopeThreshold1;

//  0x6F0126F0 - item.cpp's own redirect; re-declared here, never
//  re-thunked.
float __fastcall QueryGroundHeightHelper(int flag, void* outPtr,
                                         float x, float y, int unused);

//  0x6F2B71B0 - item_remove.cpp's own redirect, likewise.
struct SItemRemoveHelpers
{
    void SetShadowEnabled(int enabled);
};

//  0x6F333500 - item_placegate.cpp.
struct SItemPlaceGate
{
    void OnPlaced();
};

//  0x6F4C3F50 - packfourcc.cpp.  The descriptor's +0x78 is a four-character
//  *string*, not a word: this packs it into the FourCC the Position record
//  keeps at +0x88.
unsigned int __fastcall PackFourCCString(const char* text);

//  slot 89 (+0x164) - the placement mask, reached through the vtable.
typedef int (__thiscall *Slot0x164Fn)(void* self);
//  slot 82 (+0x148) - claim the pathing footprint; the counterpart of
//  RemoveFootprint (widget.h).
typedef int (__thiscall *Slot0x148Fn)(void* self, int a, int b, int c);
//  slot 79 (+0x13C) - "does this object have a real height".
typedef int (__thiscall *Slot0x13CFn)(void* self);

void CItem::PlaceFromDescriptor(unsigned int footprintType,
                                SItemPlacementDescriptor* desc)
{
#ifdef FEATURE_ITEM_CREATE_LOG
    //  Before: what the caller asked for. FindFreeWorldPoint below may move
    //  x/y, and m_z is overwritten with the sampled ground height, so the
    //  descriptor is logged again on the way out.
    FeatureItemDebug_OnItemPlaced(this, footprintType, desc, 0);
#endif

    //  Both coordinates copied out once and used three times each: as the
    //  search's in/out pair, as both corners of the degenerate box, and as
    //  the ground query's arguments.  The shipped code stores each of them
    //  into three slots up front, which is what the copies give and reading
    //  `desc->m_x` again each time does not.
    CFloat px = desc->m_x;
    CFloat py = desc->m_y;
    SCellBoxF box(py, px, py, px);

    if (FindFreeWorldPoint(&box, &px, &py, &g_slopeThreshold1,
                           ((Slot0x164Fn)(*(void***)this)[0x164 / 4])(this),
                           2, 1, 0, 0, -1, 0, 0, -1))
    {
        desc->m_x = px;
        desc->m_y = py;
    }

    //  The height comes back in st0 as a hardware float and goes straight
    //  into the descriptor's encoded m_z - the two share a bit pattern
    //  (CFloat.h), which is why the shipped code stores it with a plain
    //  integer `mov` and not through any conversion.
    float height = QueryGroundHeightHelper(-2, 0, *(const float*)&px,
                                           *(const float*)&py, 0);
    desc->m_z = *(const CFloat*)&height;

    CWidget::InitSprite(footprintType, desc, 0, -1);

    ((Slot0x148Fn)(*(void***)this)[0x148 / 4])(this, 1, 1, 1);
    ((SItemRemoveHelpers*)this)->SetShadowEnabled(1);
    ((SItemPlaceGate*)this)->OnPlaced();

    //  One OR-together of seven ternaries, not seven separate `|=` on
    //  m_flags: the shipped code builds the whole word in a register and
    //  ORs it in once.
    m_flags |= (desc->m_field74 != 0 ? 0x2000 : 0)
             | (desc->m_field70 != 0 ? 0x800 : 0)
             | (desc->m_field68 != 0 ? 0x400 : 0)
             | (desc->m_field60 != 0 ? 0x200 : 0)
             | (desc->m_field6C != 0 ? 0x80 : 0)
             | (desc->m_field64 != 0 ? 0x40 : 0)
             | (desc->m_field5C != 0 ? 0 : 0x1000);

    m_position.m_reserved84 = desc->m_field5C;
    m_position.m_reserved74 = desc->m_field58;
    m_position.m_reserved88 = PackFourCCString(desc->m_pTypeName);

    m_position.SetTopByteBits3070();

    if (((Slot0x13CFn)(*(void***)this)[0x13C / 4])(this))
        m_position.SetTopByteFlag80(1);

#ifdef FEATURE_ITEM_CREATE_LOG
    //  After: what the item was actually initialised to, and the point at
    //  which it starts being tracked for the F3 gather.
    FeatureItemDebug_OnItemPlaced(this, footprintType, desc, 1);
#endif
}
