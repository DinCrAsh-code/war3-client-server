//============================================================================
//  0x6F528200 - CRenderDevice::GetViewport: six `float*` out parameters, the
//  rectangle and the two depth planes.  Its own translation unit because
//  0x6F5266D0 is a real call to it.
//============================================================================
#include "renderdevice.h"

void CRenderDevice::GetViewport(float* outX, float* outY, float* outW,
                                float* outH, float* outNear, float* outFar)
{
    *outX    = m_viewport[0];
    *outY    = m_viewport[1];
    *outW    = m_viewport[2];
    *outH    = m_viewport[3];
    *outNear = m_viewport[4];
    *outFar  = m_viewport[5];
}
