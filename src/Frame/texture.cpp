//============================================================================
//  0x6F60D900 - CSimpleTexture::CSimpleTexture.
//
//  Every float from +0x94 to +0xE0 starts at zero and the texture coordinates
//  are then set from the unit rectangle (0, 0, 1, 1) - a stack local, which
//  is why the constructor needs a frame at all.
//
//  Score capped by the unified EH frame; see framecore.cpp.
//============================================================================
#include "frame.h"
#include "framethunks.h"


CSimpleTexture::CSimpleTexture(void* parent, int layer, int style)
    : CSimpleRegion((CSimpleFrame*)parent, layer, style)
{
    m_vertices[0]  = 0.0f;
    m_vertices[1]  = 0.0f;
    m_vertices[2]  = 0.0f;
    m_vertices[3]  = 0.0f;
    m_vertices[4]  = 0.0f;
    m_vertices[5]  = 0.0f;
    m_vertices[6]  = 0.0f;
    m_vertices[7]  = 0.0f;
    m_vertices[8]  = 0.0f;
    m_vertices[9]  = 0.0f;
    m_vertices[10] = 0.0f;
    m_vertices[11] = 0.0f;
    m_texCoord[0] = 0.0f;
    m_texCoord[1] = 0.0f;
    m_texCoord[2] = 0.0f;
    m_texCoord[3] = 0.0f;
    m_texCoord[4] = 0.0f;
    m_texCoord[5] = 0.0f;
    m_texCoord[6] = 0.0f;
    m_texCoord[7] = 0.0f;
    m_84 = 0;
    m_88 = 0;
    m_pTexture = 0;
    m_textureMode = 0;
    m_E4 = 0;

    float unit[4];
    unit[0] = 0.0f;
    unit[1] = 0.0f;
    unit[2] = 1.0f;
    unit[3] = 1.0f;
    SetTexCoords(unit);
}
