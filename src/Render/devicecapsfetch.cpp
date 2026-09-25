//============================================================================
//  0x6F526290 - the caps block, by value, off the singleton.  Two copies of
//  the same ten dwords: GetCaps fills its own hidden buffer and this one
//  copies that into the caller's, which is what MSVC emits for
//  `return device->GetCaps();` when the call's own return slot is a
//  temporary.
//============================================================================
#include "renderdevice.h"

DEVICECAPS __stdcall GetDeviceCaps()
{
    return g_renderDevice->GetCaps();
}
