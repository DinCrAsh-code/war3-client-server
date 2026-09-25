//============================================================================
//  0x6F60D4E0 - CSimpleTexture::SetTexCoordArray: take all eight coordinate
//  floats at once.  Its own name because 0x6F60D420 already carries
//  `SetTexCoords`; the two share everything but their first half.
//
//  The copies are integer `mov`s, not `fld`/`fstp` pairs, which is what says
//  the source copies whole dwords rather than floats
//  (docs/msvc-vc8-idioms.md, "An integer `mov` to copy four bytes means it
//  is not a `float`").  The tail is SetTexCoords's.
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CSimpleTexture::SetTexCoordArray(const float* coords)
{
    const int* words = (const int*)coords;
    int* dst = (int*)m_texCoord;

    dst[0] = words[0];
    dst[1] = words[1];
    dst[2] = words[2];
    dst[3] = words[3];
    dst[4] = words[4];
    dst[5] = words[5];
    dst[6] = words[6];
    dst[7] = words[7];

    if (m_E4 != 0)
    {
        float bounds[4];
        bounds[0] = 0.0f;
        bounds[1] = 0.0f;
        bounds[2] = 0.0f;
        bounds[3] = 0.0f;

        if (QueryCornerRect(bounds) != 0)
        {
            MapRect(bounds);
            SetVertices(bounds);
        }
    }
}
