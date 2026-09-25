//============================================================================
//  0x6F6033A0 - CSimpleButton's own slot 21 (+0x54): mouse-up.  Only runs
//  at all while m_enabled == 2 (i.e. while this button IS the one currently
//  pressed - slot 20's own trailing NotifyEnabledChanged(2, 0) is what sets
//  that).  Same hit test as slot 20, but the click only fires on the
//  matching "up" bit (`id << 4`, not the combined down-or-up mask), and the
//  trailing visual-state call restores state 1 (normal) rather than
//  advancing to 2.
//============================================================================
#include "frame.h"
#include "framethunks.h"

int CSimpleButton::Method_0x54(const void* msg)
{
    if (m_enabled != 2)
        return 0;

    int id = *(const int*)((const char*)msg + 0x14);
    int mask = (id << 4) | id;

    float point[2];
    point[0] = *(const float*)((const char*)msg + 0x24);
    point[1] = *(const float*)((const char*)msg + 0x28);

    if ((m_140 & mask) == 0)
        return 0;

    if (!PointInRect(point))
        return 0;

    id = *(const int*)((const char*)msg + 0x14);
    int upBit = id << 4;
    if (m_140 & upBit)
        Method_0x6Cb(id);

    if (m_13C == 0 && m_enabled != 0)
        NotifyEnabledChanged(1, 0);

    return 1;
}
