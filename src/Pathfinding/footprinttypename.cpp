//============================================================================
//  0x6F32DBE0 - a footprint type's own display name at `index`, clamped to
//  the last name it has.  See footprinttype.h.
//
//  Own translation unit for the same reason footprintshape.cpp (0x6F32D0B0)
//  and widget_footprintscale.cpp (0x6F32D070) are: the call into
//  GetOrCreateFootprintTypeData has to stay a real call rather than being
//  inlined into this four-line body.
//
//  The clamp is written as the shipped code has it - compute `count - 1`
//  first, then keep it only when it is *below* `index` - rather than as a
//  `min`.  Spelling it `index < count ? index : count - 1` inverts the
//  comparison and reloads `count`, because that order asks about `index`
//  first and this one asks about `count - 1` first.
//
//  `last += -1` rather than `last = last - 1`: the shipped code decrements
//  with `add ecx, -1`, and only the compound-assign spelling picks that
//  encoding here - plain subtraction emits `sub ecx, 1`.
//============================================================================
#include "footprinttype.h"

const char* __fastcall GetFootprintTypeName(unsigned int id, unsigned int index)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "name");
    if (data != 0)
    {
        unsigned int last = data->m_nameCount;
        if (last != 0)
        {
            --last;
            if (!(last < index))
                last = index;
            return data->m_names[last];
        }
    }
    return "Default string";
}
