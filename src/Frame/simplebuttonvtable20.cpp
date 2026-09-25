//============================================================================
//  0x6F603300 - CSimpleButton's own slot 20 (+0x50): mouse-down.  `msg` is
//  the same message shape slot 14/HitTestMessage reads (a raw button/id at
//  +0x14, a point at +0x24/+0x28).  m_140 packs two bitmasks per button -
//  bit N for "down", bit N+4 for "up" (the `(id << 4) | id` combined test
//  below) - so a button not listening for either variant of `id` bails out
//  before ever hit-testing the point.  On an actual hit it always fires
//  slot 27 (click) when the raw "down" bit is set, then - once, the first
//  time m_13C is still 0 - tells slot 28 to switch to the pressed visual
//  (state 2).
//============================================================================
#include "frame.h"
#include "framethunks.h"

int CSimpleButton::Method_0x50(const void* msg)
{
    if (m_enabled == 0)
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

    int result = 1;

    id = *(const int*)((const char*)msg + 0x14);
    if (m_140 & id)
        Method_0x6Cb(id);

    if (m_13C == 0 && m_enabled != 0)
        NotifyEnabledChanged(2, 0);

    return result;
}
