//============================================================================
//  0x6F4DDFB0 - halve both dimensions until they fit what the device can
//  take, counting the halvings into `mipSkip` so the decoder knows how many
//  levels to throw away.  Neither dimension is allowed to reach zero.
//
//  **The height arrives in ebx in the shipped build** - it is never written
//  before its first read and the one call site loads exactly ebx - so it is
//  the ecx parameter here and the caller pays one move.  The caps block is
//  fetched by value and the limit read out of it once, which is why the
//  loop head is the *width* load and not the limit's.
//============================================================================
#include "texture.h"
#include "renderdevice.h"

void __fastcall ShrinkTextureToDeviceLimit(unsigned int* height,
                                           TEXTUREDIMREF widthRef,
                                           TEXTUREMIPREF mipSkipRef)
{
    unsigned int* width = widthRef.m_value;
    int* mipSkip = mipSkipRef.m_value;

    DEVICECAPS caps = GetDeviceCaps();
    unsigned int limit = caps.m_maxTextureDim;

    while (*width > limit || *height > limit)
    {
        *width  = *width  >> 1;
        *height = *height >> 1;
        *mipSkip += 1;

        if (*width == 0)
            *width = 1;
        if (*height == 0)
            *height = 1;
    }
}
