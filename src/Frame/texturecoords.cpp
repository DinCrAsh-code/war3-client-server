//============================================================================
//  0x6F60D420 - CSimpleTexture::SetTexCoords.
//
//  One (x0, y0, x1, y1) rectangle spread over the four corners as (v, u)
//  pairs: (y0,x0), (y0,x1), (y1,x0), (y1,x1).  Each pair is written through
//  the two-argument helper below, which is what puts the single spilled
//  temporary in the stream - MSVC evaluates the arguments right to left, so
//  the second component is computed and parked before the first is even
//  loaded.
//
//  Its own translation unit because the constructor calls it.
//============================================================================
#include "frame.h"
#include "framethunks.h"


static void SetCorner(float* corner, float v, float u)
{
    corner[0] = v;
    corner[1] = u;
}

void CSimpleTexture::SetTexCoords(const float* rect)
{
    SetCorner(&m_texCoord[0], rect[1], rect[0]);
    SetCorner(&m_texCoord[2], rect[1], rect[2]);
    SetCorner(&m_texCoord[4], rect[3], rect[0]);
    SetCorner(&m_texCoord[6], rect[3], rect[2]);

    if (m_E4)
    {
        float corners[4];
        corners[0] = 0.0f;
        corners[1] = 0.0f;
        corners[2] = 0.0f;
        corners[3] = 0.0f;

        if (QueryCornerRect(corners))
        {
            MapRect(corners);
            SetVertices(corners);
        }
    }
}
