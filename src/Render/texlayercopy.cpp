//============================================================================
//  0x6F4F6B80 - CTexLayer's copy constructor: seven plain dwords copied one
//  at a time, then the two texture pointers taken a reference on in a
//  two-turn loop off a walking pointer.
//
//  The loop is the shipped code's own shape - `mov ecx, [ebx+esi]` with ebx
//  holding `src - this`, so one register indexes both objects - and that is
//  what makes it a loop rather than two lines.
//============================================================================
#include "material.h"
#include "refcnt.h"

CTexLayer::CTexLayer(const CTexLayer& other)
{
    m_reserved00[0] = other.m_reserved00[0];
    m_reserved00[1] = other.m_reserved00[1];
    m_reserved00[2] = other.m_reserved00[2];
    m_reserved00[3] = other.m_reserved00[3];
    m_alphaMode     = other.m_alphaMode;
    m_reserved14[0] = other.m_reserved14[0];
    m_reserved14[1] = other.m_reserved14[1];

    for (int i = 0; i < 2; i++)
        m_textures[i] = (CTexture*)((TRefCnt*)other.m_textures[i])->AddRefOrNull();
}
