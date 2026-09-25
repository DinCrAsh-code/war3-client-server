//============================================================================
//  0x6F60CD50 - CSimpleTexture::SetVertices: build the quad's four (x, y, z)
//  vertices out of a rect.
//
//  One reused scratch float, which is what the shipped code's single
//  argument-slot temporary is; four separate locals would take four slots.
//============================================================================
#include "frame.h"
#include "framethunks.h"

void CSimpleTexture::SetVertices(const float* rect)
{
    float t;

    t = rect[2];
    m_vertices[0] = rect[1];
    m_vertices[1] = t;
    m_vertices[2] = 0.0f;

    t = rect[0];
    m_vertices[3] = rect[1];
    m_vertices[4] = t;
    m_vertices[5] = 0.0f;

    t = rect[2];
    m_vertices[6] = rect[3];
    m_vertices[7] = t;
    m_vertices[8] = 0.0f;

    t = rect[0];
    m_vertices[9] = rect[3];
    m_vertices[10] = t;
    m_vertices[11] = 0.0f;
}
