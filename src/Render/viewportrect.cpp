//============================================================================
//  0x6F5266D0 - the viewport, forwarded to the singleton.  The first two out
//  parameters ride in ecx and edx, which is what makes it `retn 10h` where
//  the device's own accessor is `retn 18h`.
//============================================================================
#include "renderdevice.h"

void __fastcall GetViewportRect(float* outX, float* outY, float* outW,
                                float* outH, float* outNear, float* outFar)
{
    g_renderDevice->GetViewport(outX, outY, outW, outH, outNear, outFar);
}
