//============================================================================
//  CUnit batch-14 (slot-3 closure, off the message dispatcher 0x6F2A7E60).
//
//  0x6F32DC30 - a footprint type's own "specialArt" name at `index`,
//  clamped to the last one it has - the same clamp-and-fetch shape
//  GetFootprintTypeName (footprinttypename.cpp) already established one
//  property earlier (m_nameCount/m_names), just against a second counted
//  array (m_specialArtCount/m_specialArt) and with a null rather than
//  "Default string" fallback: this one returns 0 whenever the type has no
//  record at all *or* its own array is empty, where GetFootprintTypeName
//  only falls back on the former.
//
//  Same "count - 1 first, kept only when below index" clamp order as
//  GetFootprintTypeName - see that file's own note on why the comparison
//  cannot be flipped to a `min` without changing which operand reloads
//  first.
//============================================================================
#include "footprinttype.h"

const char* __fastcall GetFootprintSpecialArt(unsigned int id, unsigned int index)
{
    CFootprintTypeData* data = GetOrCreateFootprintTypeData(id, "specialArt");
    if (data != 0)
    {
        unsigned int last = data->m_specialArtCount;
        if (last != 0)
        {
            --last;
            if (!(last < index))
                last = index;
            return data->m_specialArt[last];
        }
    }
    return 0;
}
