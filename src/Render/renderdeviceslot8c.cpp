//============================================================================
//  0x6F526080 - forward two floats through the render device's own vtable
//  slot 0x8C.  The dump's own FLD/FSTP round-trip is exactly what MSVC
//  emits to pass two `float` arguments to a `__thiscall` callee on the
//  stack (each reloaded through the x87 stack rather than copied as raw
//  bytes), so an ordinary two-float call reproduces it - matching
//  renderdevicesubmit.cpp's own vtable-forwarding shape for slot 0xA8.
//============================================================================
#include "renderdevice.h"

void __stdcall RenderDeviceSlot0x8CForward(float a, float b)
{
    typedef void (__thiscall *Slot8CFn)(CRenderDevice*, float, float);
    ((Slot8CFn)(*(void***)g_renderDevice)[0x8C / 4])(g_renderDevice, a, b);
}
