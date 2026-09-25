//============================================================================
//  0x6F526510 - hand one texture creation to the render device.
//
//  The device's own vtable slot 0xA8 takes seven arguments and cleans them
//  (`retn 1Ch`); this forwarder takes the first two in ecx and edx and the
//  other five on the stack, which is what makes it `retn 14h`.  The result
//  slot is cleared before the call rather than after it, which is the
//  shipped order.
//============================================================================
#include "renderdevice.h"

int __fastcall SubmitTextureCreate(unsigned int width, unsigned int height,
                                   int format, TEXTUREOPTIONS options, void* texture,
                                   const void* codec, void** outImage)
{
    *outImage = 0;

    typedef int (__thiscall *CreateTextureFn)(CRenderDevice*, unsigned int,
                                              unsigned int, int, TEXTUREOPTIONS, void*,
                                              const void*, void**);
    return ((CreateTextureFn)(*(void***)g_renderDevice)[0xA8 / 4])(
        g_renderDevice, width, height, format, options, texture, codec,
        outImage);
}
