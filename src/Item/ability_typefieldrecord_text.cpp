//============================================================================
//  0x6F330580 - SAbilityFieldRecordNode::AppendRequiresText, CAbilityButton's
//  own tooltip-building leaf (Method_0x298, abilitybutton_bigslots.cpp).
//
//  Own translation unit for the same reason ability_typefieldrecord.cpp's
//  own LookupAbilityFieldRecord is: this is a real out-of-line call from
//  Method_0x298's own dump, not something to fold back into the same TU
//  as its caller.
//
//  Two near-identical passes - required/optional first, required2/
//  optional2 second, unconditionally (the second pass is not gated on
//  whether the first group had any entries, only on m_requiredCount
//  itself being nonzero at all) - each: clamp `index` against the
//  group-count field, read that group's own `m_count`, then for every
//  item 0..m_count-1 re-clamp `index` against the count *again* (the
//  shipped code re-reads and re-clamps from scratch every iteration
//  rather than hoisting a loop-invariant, the same "no aliasing proof
//  across an indirect array read" shape GetFootprintTypeName's own
//  header documents) to fetch a required type id, does the identical
//  clamp-then-read against the optional group for a name-index, and
//  formats "\n - %s" of GetFootprintTypeName(typeId, nameIndex - 1) onto
//  `buf` when that name is non-null.
//============================================================================
#include "abilityfieldrecordtable.h"
#include "footprinttype.h"
#include "text.h"
#include "os.h"

//  Both groups clamp `groupIndex` against `groupCount` and hand back the
//  selected group's own item count - the same single-clamp shape
//  GetFootprintTypeName (footprinttype.h) already has, just reading
//  `m_count` rather than a name pointer.
static unsigned int ClampedGroupCount(
    unsigned int groupCount,
    const SAbilityFieldRecordNode::SGroup* groups,
    unsigned int groupIndex)
{
    if (groupCount == 0)
        return 0;
    unsigned int gi = groupIndex;
    if (groupCount - 1 < gi)
        gi = groupCount - 1;
    return groups[gi].m_count;
}

//  The item read itself: re-clamps `groupIndex` against `groupCount`
//  (freshly, matching the shipped code's own redundant re-read) and, when
//  that group has any items, clamps `itemIndex` against its own count and
//  reads `m_data[itemIndex]` - `fallback` otherwise.
static unsigned int ClampedGroupItem(
    unsigned int groupCount,
    const SAbilityFieldRecordNode::SGroup* groups,
    unsigned int groupIndex,
    unsigned int itemIndex,
    unsigned int fallback)
{
    if (groupCount == 0)
        return fallback;
    unsigned int gi = groupIndex;
    if (groupCount - 1 < gi)
        gi = groupCount - 1;
    unsigned int itemCount = groups[gi].m_count;
    if (itemCount == 0)
        return fallback;
    unsigned int ii = itemIndex;
    if (itemCount - 1 < ii)
        ii = itemCount - 1;
    return groups[gi].m_data[ii];
}

void SAbilityFieldRecordNode::AppendRequiresText(
    char* buf, unsigned int bufSize, unsigned int index)
{
    *buf = 0;
    if (m_requiredCount == 0)
        return;

    unsigned int reqGroupCount = ClampedGroupCount(m_requiredCount, m_required, index);
    if (reqGroupCount != 0)
    {
        for (unsigned int i = 0; i < reqGroupCount; ++i)
        {
            unsigned int typeId = ClampedGroupItem(m_requiredCount, m_required, index, i, 0);
            unsigned int nameIndex = ClampedGroupItem(m_optionalCount, m_optional, index, i, 1);

            const char* name = GetFootprintTypeName(typeId, nameIndex - 1);
            if (name != 0)
            {
                char line[0x80];
                Storm_578(line, 0x80, "\n - %s", name);
                Storm_503(buf, line, bufSize);
            }
        }
    }

    unsigned int req2GroupCount = ClampedGroupCount(m_required2Count, m_required2, index);
    if (req2GroupCount != 0)
    {
        for (unsigned int i = 0; i < req2GroupCount; ++i)
        {
            unsigned int typeId = ClampedGroupItem(m_required2Count, m_required2, index, i, 0);
            unsigned int nameIndex = ClampedGroupItem(m_optional2Count, m_optional2, index, i, 1);

            const char* name = GetFootprintTypeName(typeId, nameIndex - 1);
            if (name != 0)
            {
                char line[0x80];
                Storm_578(line, 0x80, "\n - %s", name);
                Storm_503(buf, line, bufSize);
            }
        }
    }
}
