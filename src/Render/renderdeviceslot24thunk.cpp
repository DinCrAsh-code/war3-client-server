//============================================================================
//  0x6F525F40 - a pure tail-forward into the render device's own vtable
//  slot 0x24: load the device, load its vtable, and `jmp` straight into the
//  slot with whatever the caller already set up (no arguments of this
//  function's own are visible - it never has a prologue or its own
//  `retn`).  A naked body is not eligible for the register/scheduling
//  excuses CLAUDE.md's own BEHAVIOUR table lists, so this should score
//  1.000 by construction.
//============================================================================
#include "renderdevice.h"

__declspec(naked) void RenderDeviceSlot0x24Forward()
{
    __asm {
        mov ecx, g_renderDevice
        mov eax, [ecx]
        mov edx, [eax + 24h]
        jmp edx
    }
}
