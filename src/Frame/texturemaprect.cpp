//============================================================================
//  0x6F60CCD0 - CSimpleTexture::MapRect: map a rect through the four texture
//  coordinates, in place.
//
//  Every intermediate is a named local because the shipped code stores all
//  four to memory before it starts: the two origins and the two extents.
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CSimpleTexture::MapRect(float* rect)
{
    float y0 = rect[1];
    float x0 = rect[0];
    float dy = rect[3] - rect[1];
    float dx = rect[2] - rect[0];

    rect[1] = y0 + m_texCoord[0] * dy;
    rect[3] = y0 + m_texCoord[4] * dy;
    rect[0] = x0 + m_texCoord[1] * dx;
    rect[2] = x0 + m_texCoord[3] * dx;
}
