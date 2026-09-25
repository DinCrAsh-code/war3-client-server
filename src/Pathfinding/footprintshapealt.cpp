//============================================================================
//  0x6F32D0D0 - a type's *alternate* pathing footprint shape.
//
//  Byte-for-byte the same shape as GetFootprintShape (0x6F32D0B0), reading
//  the "pPathingFootprintAlt" object-editor field instead of the plain one
//  and the same +0xD0 slot off the type data.  Two separate entry points in
//  the shipped binary rather than one with a field-name argument, so two
//  here.
//
//  Own translation unit: CWidget::GetFootprintShape (0x6F2AC340,
//  widgetfootprint.cpp) calls it for real.
//============================================================================
#include "footprinttype.h"

CFootprintShape* __fastcall GetFootprintShapeAlt(unsigned int id)
{
    CFootprintTypeData* data =
        GetOrCreateFootprintTypeData(id, "pPathingFootprintAlt");
    if (data == 0)
        return 0;
    return *(CFootprintShape**)((char*)data + 0xD0);
}
