//============================================================================
//  0x6F526DF0 - tear the render device down (`shouldReset` guards the whole
//  body - a no-op call with it false).  Clears the cursor clip rect
//  (sub_6F525F90, shared with renderdeviceswitch.cpp's own use), notifies
//  through Storm ordinal 470, calls the device's own vtable slots 0x10 and
//  8 (the second only if the device pointer is still live, one stack
//  argument - matches renderdevicesubmit.cpp's own slot-0xA8 forwarding
//  shape), then zeroes the device pointer, its shadow field at +8, and
//  resets a screen-status pair to (-1, 2).
//============================================================================
#include "renderdevice.h"

//  0x6F525F90 is GameMain_sub_6F525F90 (Misc/gamemainenginethunks.cpp) - a
//  cursor-clip helper, `retn 0`, single register argument, no stack.  This
//  file used to carry its own second, naked redirect to the same address
//  under the name ClearCursorClipRect (shared with renderdeviceswitch.cpp's
//  own use) - a duplicate; both call sites now go through the canonical
//  name instead.
void __fastcall GameMain_sub_6F525F90(unsigned int);

//  Storm ordinal 470 - unresolved real name.  One stack argument (the
//  address of a shared static continuation, itself a naked redirect to the
//  real function since nothing in this call tree needs its body - see
//  RenderDeviceResetContinuation below).
void __stdcall Storm_470(void* callback);

extern void* g_renderDeviceShadow;    // dword_6FACBD48
extern int   g_screenStatusA;         // dword_6FA88720
extern int   g_screenStatusB;         // dword_6FA88724

//  0x6F526930 - a callback Storm_469/Storm_470 are handed by address only;
//  never called from anywhere in this call tree, so only its address
//  matters.  Below this batch's own 43; declared and thunked to its own
//  real address.
__declspec(naked) void RenderDeviceResetContinuation()
{
    __asm { mov eax, 06F526930h }
    __asm { jmp eax }
}

void __fastcall ShutdownRenderDevice(int shouldReset)
{
    if (!shouldReset)
        return;

    GameMain_sub_6F525F90(0);
    Storm_470((void*)&RenderDeviceResetContinuation);

    typedef void (__thiscall *Slot10Fn)(CRenderDevice*);
    ((Slot10Fn)(*(void***)g_renderDevice)[0x10 / 4])(g_renderDevice);

    if (g_renderDevice != 0)
    {
        typedef void (__thiscall *Slot8Fn)(CRenderDevice*, int);
        ((Slot8Fn)(*(void***)g_renderDevice)[8 / 4])(g_renderDevice, 1);
    }

    g_renderDevice = 0;
    g_renderDeviceShadow = 0;
    g_screenStatusA = -1;
    g_screenStatusB = 2;
}
