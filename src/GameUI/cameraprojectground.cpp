//============================================================================
//  0x6F303070 - CCameraWar3::ProjectGroundPoint: take a requested ground
//  point, clamp it to the camera's own bounds, and put the terrain height
//  under it into the third component.
//
//  Three stages, all in hardware floats:
//
//   1. the point goes through the first of the camera's two 3x3 matrices
//      (+0x528), which takes it into the space the bounds at +0x4CC..+0x4D8
//      are expressed in.  Only the first two rows are read and the input's
//      third component is a literal zero at every use, which is why the
//      shipped code has an `fldz` feeding two multiplies that can only ever
//      produce zero - it is a full 3x3 transform of (x, y, 0), not a 2x2;
//   2. each component is clamped: below the minimum it becomes the minimum,
//      above the maximum it becomes the maximum, and the shipped code
//      spells that as `if (min > v) v = min; else if (max < v) v = max;` -
//      one comparison against each bound with the *bound* on the left, which
//      is what puts `fcomp st(1)` and `fcompp` in that order.  Then the
//      second matrix (+0x54C) takes the clamped point back out again;
//   3. the height is a **nine-tap sample** of the terrain on a 3x3 grid
//      500 units apart around the result, weighted by the kernel at +0x570
//      that CCameraWar3::ResetToDefaults fills in, and divided by the sum of
//      those weights.  The two offsets come out of the loop counter as
//      `1 - i/3` and `1 - i%3` - integers, which is why one of them reaches
//      the FPU through `fild` and the other through `fimul`; the shipped
//      `mul 0AAAAAAABh` / `shr edx,1` is the compiler's own division by
//      three and not something the source spells.
//
//  Own translation unit: nine real calls out of the loop.
//============================================================================
#include "gameui.h"

//  0x6F0126F0 - item.cpp's own redirect; re-declared here, never re-thunked
//  (jasslocationheight.cpp and item_place.cpp do the same).
float __fastcall QueryGroundHeightHelper(int flag, void* outPtr,
                                         float x, float y, int unused);

//  flt_6F97FF58 - five hundred, the spacing of the nine taps; and
//  dword_6F97FF54 - the query mode the sample asks for, a *global* and not
//  an immediate in the shipped code.
extern const float g_groundSampleSpacing;
extern const int   g_groundSampleQueryMode;

float* CCameraWar3::ProjectGroundPoint(float* out, const float* point)
{
    out[0] = 0.0f;
    out[1] = 0.0f;
    out[2] = 0.0f;

    float px = point[0];
    float py = point[1];

    //  The third row of both matrices is fed a literal zero - see above.
    float ax = m_toBoundsSpace[0] * px + m_toBoundsSpace[3] * py +
               m_toBoundsSpace[6] * 0.0f;
    float ay = m_toBoundsSpace[1] * px + m_toBoundsSpace[4] * py +
               m_toBoundsSpace[7] * 0.0f;

    px = ax;
    py = ay;

    if (m_boundsMinX > px)
        px = m_boundsMinX;
    else if (m_boundsMaxX < px)
        px = m_boundsMaxX;

    if (m_boundsMinY > py)
        py = m_boundsMinY;
    else if (m_boundsMaxY < py)
        py = m_boundsMaxY;

    out[0] = m_fromBoundsSpace[0] * px + m_fromBoundsSpace[3] * py +
             m_fromBoundsSpace[6] * 0.0f;
    out[1] = m_fromBoundsSpace[1] * px + m_fromBoundsSpace[4] * py +
             m_fromBoundsSpace[7] * 0.0f;

    float total = 0.0f;
    float weight = 0.0f;

    for (int i = 0; i < 9; i++)
    {
        float w = m_sampleWeights[i];
        weight = w + weight;

        int stepY = 1 - i % 3;
        int stepX = 1 - i / 3;

        float sy = (float)stepY * g_groundSampleSpacing + out[1];
        float sx = g_groundSampleSpacing * stepX + out[0];

        total = QueryGroundHeightHelper(g_groundSampleQueryMode, 0, sx, sy, 0)
                * w + total;
    }

    out[2] = total / weight;
    return out;
}
