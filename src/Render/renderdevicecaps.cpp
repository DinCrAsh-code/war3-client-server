//============================================================================
//  0x6F527260 - CRenderDevice::GetCaps.
//
//  It returns the ten-dword block by value, which at this ABI means the
//  caller hands down a hidden buffer as the last stack argument and the
//  callee pops it (`retn 4`) and answers it in eax.  The `rep movsd` is the
//  struct assignment; nothing here calls memcpy.
//
//  Its own translation unit because GetDeviceCaps (0x6F526290) is a real
//  call to it in the shipped code, and a visible definition would let /Ob2
//  inline it there.
//============================================================================
#include "renderdevice.h"

DEVICECAPS CRenderDevice::GetCaps()
{
    return m_caps;
}
