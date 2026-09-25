//============================================================================
//  0x6F2B5C60 - CItem's vtable slot 91 (+0x16C).  See item.h.
//
//  Ask this object for its own placement radius through slot 90 (+0x168,
//  CWidget::GetPlacementRadius answers encoded 1.0), clamp it *up* to 1.0,
//  hand the clamped value to m_position, and finish by folding slot 88's
//  answer into m_position's own low-24-bit word - the same
//  SHandleWithType::SetLow24Bits (widget_handlebits.cpp) CWidget's slot 87
//  uses, which is a second, independent confirmation that Position's first
//  two words really are a {handle, typeTag} pair.
//
//  The clamp is written as a ternary picking between two *addresses*, not
//  between two values: the shipped code materialises `&radius` or
//  `&g_slopeThreshold1` into eax and only then loads through it.
//============================================================================
#include "item.h"
#include "timesyncbounds.h"

extern const CFloat g_slopeThreshold1;   // dword_6FAAE4F4 - encoded 1.0

//  slot 90 (+0x168) - CWidget::GetPlacementRadius, through the vtable
//  because CItem does not override it and a derived class might.
typedef CFloat* (__thiscall *Slot0x168Fn)(void* self, CFloat* out);
//  slot 88 (+0x160) - CItem::Method_0x160 (item_vtable.cpp), 0x18.
typedef unsigned int (__thiscall *Slot0x160Fn)(void* self);

//  0x6F4766A0 - hand the clamped radius to the position record.
//  Reconstructed in positionradiussink.cpp; declared, not defined, here so
//  this call site reaches it with a real call.  A real thiscall member:
//  one stack argument, `this` in ecx.
struct SPositionRadiusSink
{
    void SetRadius(const CFloat* radius);
};

void CItem::RefreshPlacementRadius()
{
    CFloat radius(kCFloatNoInit);
    ((Slot0x168Fn)(*(void***)this)[0x168 / 4])(this, &radius);

    //  `radius > 1.0` and not `1.0 < radius`: the shipped compare loads the
    //  radius first and masks with `test ah,5` / `jnp`, which is MSVC's
    //  `>`; the reversed spelling loads the constant first and uses
    //  `test ah,41h`.
    const CFloat* clamped =
        (*(const float*)&radius > *(const float*)&g_slopeThreshold1)
            ? &radius : &g_slopeThreshold1;
    radius = *clamped;

    ((SPositionRadiusSink*)&m_position)->SetRadius(&radius);

    ((SHandleWithType*)&m_position)->SetLow24Bits(
        ((Slot0x160Fn)(*(void***)this)[0x160 / 4])(this));
}
