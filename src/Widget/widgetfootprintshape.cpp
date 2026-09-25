//============================================================================
//  0x6F2AC340 - the widget's own pathing footprint shape, or null.
//
//  Gated on vtable[0xB0]: a widget that answers zero there has no footprint
//  at all and the caller does nothing.  Otherwise the shape comes from the
//  widget's *footprint type* - the bare FourCC at CWidget+0x30 that
//  CWidget::InitSprite records (widget.h) - and the flag picks which of the
//  two object-editor fields to read it from.
//
//  Own translation unit: both lookups are real calls.  A thiscall member,
//  not a __fastcall free function - the flag is pushed and cleaned
//  (`retn 4`), never passed in edx.
//============================================================================
#include "widget.h"
#include "widgetfootprint.h"

typedef int (__thiscall *HasFootprintFn)(void*);

CFootprintShape* CWidget::GetOwnFootprintShape(int useAlternate)
{
    void* vself = this;
    if (((HasFootprintFn)(*(void***)vself)[0xB0 / 4])(vself) == 0)
        return 0;

    if (useAlternate)
        return GetFootprintShape(m_footprintType);
    return GetFootprintShapeAlt(m_footprintType);
}
