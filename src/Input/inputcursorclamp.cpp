//============================================================================
//  0x6F630C20 - ClampCursorToScreenBounds.  Shared by InputMouseDownHandler
//  (type 9) and four of the other dispatch types (10, 11, 13 call it for
//  its cursor-warp side effect; type 12 inlines just the float conversion
//  half without the clamp).  In every one of those callers its own two
//  fractional outputs are computed and then never read again - the shipped
//  compiler keeps the computation anyway (an FP truncation can raise an
//  exception, so it is not provably dead), and this reconstruction
//  preserves the same shape rather than "cleaning it up".
//
//  Uses the already-reconstructed `SScreenRect::Contains`
//  (GameUI/gameui.h, 0x6F332540) against a global rect
//  (g_pumpClampRect, flt_6FACEBB0) this module owns: if the raw point is
//  already inside it, nothing happens; otherwise the point is clamped into
//  the rect - inset by one pixel on the low bound of each axis, a margin
//  against the warped cursor landing exactly on the boundary and
//  re-triggering this same check - and the OS cursor is warped there via
//  WarpCursorTo.  The exact x87 min/max reassociation this session's own
//  time budget did not verify bit-for-bit; scored and marked accordingly.
//============================================================================
#include "inputeventpump.h"
#include "../Window/wininputbridge.h"

void __fastcall ClampCursorToScreenBounds(int x, int y, float* outFracX,
                                          float* outFracY)
{
    float heightRange = g_pumpClampRect.m_maxY - g_pumpClampRect.m_minY;
    if (heightRange > 0.0f)
    {
        float widthRange = g_pumpClampRect.m_maxX - g_pumpClampRect.m_minX;
        if (widthRange > 0.0f)
        {
            float point[2];
            point[0] = (float)x;
            point[1] = (float)y;

            if (!g_pumpClampRect.Contains(point))
            {
                float minXInset = g_pumpClampRect.m_minX + 1.0f;
                float clampedX = (point[0] < minXInset) ? minXInset : point[0];
                if (clampedX > g_pumpClampRect.m_maxX)
                    clampedX = g_pumpClampRect.m_maxX;

                float minYInset = g_pumpClampRect.m_minY + 1.0f;
                float clampedY = (point[1] < minYInset) ? minYInset : point[1];
                if (clampedY > g_pumpClampRect.m_maxY)
                    clampedY = g_pumpClampRect.m_maxY;

                WarpCursorTo((int)clampedX, (int)clampedY);
            }
        }
    }

    int rect[4];
    GetCachedClientRect(rect);

    *outFracX = (float)x / (float)(rect[2] - rect[0]);
    *outFracY = 1.0f - (float)y / (float)(rect[3] - rect[1]);
}
