//============================================================================
//  0x6F2B7B60 - CItem's override of vtable slot 16 (+0x40), where a plain
//  widget has 0x6F2AB980 (widget_slot40.cpp).
//
//  Everything the base does, then re-seat the item: unless something already
//  owns its placement ref, push it back at its own current world position
//  through JASS_SetItemPosition_callee1; then rebuild the shadow sprite
//  (destroy, then create), release the model instance, rewrite the pathing
//  record's flag byte, and re-assert the walkable bit if slot 79 says the
//  item has a real height.
//
//  Own translation unit: eight real calls out of it, one of them the base
//  class's own body by name rather than through the vtable - which is what
//  `CWidget::RefreshOwningPlayerColor()` compiles to and what a virtual dispatch does
//  not.
//============================================================================
#include "item.h"
#include "sprite.h"
#include "itemhandleresolve.h"  // QueryHandleField0x54
#include "game.h"               // CPathRef::SmartPtrToWorldVec3, CWorldVec3

//  0x6F2B71B0 - item_remove.cpp's own redirect; re-declared, never
//  re-thunked.
struct SItemRemoveHelpers
{
    void SetShadowEnabled(int enabled);
};

//  0x6F01F450 - the model-release callback whose *address* is handed to
//  ReleaseSpriteModel and never called (widgetsprite.cpp).
void __fastcall ModelReleaseNotify();

//  slot 46 (+0xB8) - the handle-bearing sub-object.
typedef void* (__thiscall *GetHandleObjectFn)(void*);
//  slot 79 (+0x13C) - CWidget::Method_0x13C, "does this object have a real
//  height".
typedef int (__thiscall *Slot0x13CFn)(void* self);

void CItem::RefreshOwningPlayerColor()
{
    CWidget::RefreshOwningPlayerColor();

    //  The same `(handle & typeTag) != -1` sentinel test as everywhere else
    //  (itemhandleresolve.h), and the same fall-through: an unset ref skips
    //  the lookup and lands on the same "nobody owns me" path a lookup that
    //  came back null does.
    SOptionalHandleRef* ref = &m_position.m_placementRef;
    void* owner = 0;
    if ((int)(ref->m_handle & ref->m_typeTag) != -1)
        owner = QueryHandleField0x54(ref);

    if (owner == 0)
    {
        //  Copied out of the returned buffer into a local of its own: the
        //  shipped code moves all three components across (`mov` each) even
        //  though only the first two are then passed, which is a whole
        //  CWorldVec3 copy and not two component reads.
        CWorldVec3 raw =
            ((const CPathRef*)((GetHandleObjectFn)(*(void***)this)[0xB8 / 4])(this))
                ->SmartPtrToWorldVec3();

        //  A second, explicitly built vector rather than passing `raw`'s own
        //  members: the shipped code moves all three components out of the
        //  return buffer into three fresh slots (`mov` each, including the
        //  z it never passes) before the call, which is a real copy and not
        //  two component reads off the buffer.
        CWorldVec3 here(raw.m_x, raw.m_y, raw.m_z);

        JASS_SetItemPosition_callee1(&here.m_x, &here.m_y, -1);
    }

    //  Destroy then recreate, not a refresh: two calls with 0 and then 1.
    ((SItemRemoveHelpers*)this)->SetShadowEnabled(0);
    ((SItemRemoveHelpers*)this)->SetShadowEnabled(1);

    if (m_pSprite != 0)
        ReleaseSpriteModel(m_pSprite, (void*)ModelReleaseNotify);

    m_position.SetTopByteBits3070();

    if (((Slot0x13CFn)(*(void***)this)[0x13C / 4])(this))
        m_position.SetTopByteFlag80(1);
}
