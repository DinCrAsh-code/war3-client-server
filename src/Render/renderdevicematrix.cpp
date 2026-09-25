//============================================================================
//  0x6F527310 / 0x6F527330 - the projection and view matrices, copied out of
//  the device into the caller's buffer.  A struct assignment each; the two
//  differ only in the field and the length.
//============================================================================
#include "renderdevice.h"

void CRenderDevice::GetProjectionMatrix(FMATRIX44* out)
{
    *out = m_projection;
}

void CRenderDevice::GetViewMatrix(FMATRIX43* out)
{
    *out = m_view;
}
