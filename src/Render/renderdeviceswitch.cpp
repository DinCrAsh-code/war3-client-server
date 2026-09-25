//============================================================================
//  0x6F526D10 - (re)create the render device for one of a table of modes,
//  through the device's own vtable slot 0xC; on failure (a nonzero return
//  from that slot) show a prompt (Storm ordinal 469's own continuation,
//  the same 0x6F526930 address ShutdownRenderDevice's own Storm_470 call
//  hands over) and remember the failing mode's width/height before
//  returning the device pointer anyway; on success, release slot 8 first
//  and return null.
//============================================================================
#include "renderdevice.h"

//  0x6F52A0C0 / 0x6F52B1C0 are GameMain_sub_6F52A0C0/GameMain_sub_6F52B1C0
//  (Misc/gamemainenginethunks.cpp) - the two device constructors this
//  switch picks between (`kind`==2 vs everything else, `kind`==0 taking the
//  other of the two), both `retn 0`, no arguments.  This file used to carry
//  its own second, naked pair of redirects to the same two addresses under
//  the names CreateRenderDeviceKindB/CreateRenderDeviceKindA - a duplicate;
//  the call sites below now go through the canonical names instead.
void* __cdecl GameMain_sub_6F52A0C0();
void* __cdecl GameMain_sub_6F52B1C0();

//  0x6F525F90 is GameMain_sub_6F525F90 (Misc/gamemainenginethunks.cpp), the
//  same cursor-clip helper renderdeviceshutdown.cpp calls.
void __fastcall GameMain_sub_6F525F90(unsigned int);

void __stdcall Storm_469(void* callback);
void RenderDeviceResetContinuation();   // renderdeviceshutdown.cpp, 0x6F526930

//  0x6F526C20 is CallRenderDeviceSlot0x3C (Render/renderdeviceslot3c44.cpp,
//  gamemain-batch-3's own real reconstruction of this address - a stdcall,
//  one plain stack dword, `retn 4`).  This TU used to carry a second, naked
//  redirect to the same address under the name ReportDeviceCreateFailure,
//  declared `unsigned int*` and called as `ReportDeviceCreateFailure(&
//  failureTag)` below - a duplicate of the real body *and* a wrong
//  signature: the dump's own call site (`push ecx; mov eax,esp; mov [eax],
//  0FF000000h; call`) passes the constant 0xFF000000 by value on the stack,
//  not the address of a local, so the old call was handing the real
//  function a stack pointer instead of the tag.  Fixed by calling the real
//  reconstruction directly with the value.
void __stdcall CallRenderDeviceSlot0x3C(unsigned int value);   // renderdeviceslot3c44.cpp, 0x6F526C20

//  0x6F5264F0 is CallRenderDeviceSlot0x44 (Render/renderdeviceslot3c44.cpp,
//  gamemain-batch-3's own real reconstruction of this address).  This file
//  used to carry a second, naked redirect to the same address under the
//  name RenderDeviceSwitchStep - a duplicate; both call sites below (`ecx`=1
//  each time) now go through the real reconstruction instead.
void __fastcall CallRenderDeviceSlot0x44(unsigned int value);   // renderdeviceslot3c44.cpp, 0x6F5264F0

extern int g_failedHeight;      // dword_6FACBD3C
extern int g_failedWidth;       // dword_6FACBD38
extern int g_screenModeTable[]; // dword_6FACBD4C
extern int g_screenStatusA;     // dword_6FA88720
extern int g_screenStatusB;     // dword_6FA88724

//  ecx/edx are two register-passed values this call tree never gives an
//  identifying use for (never read back after the vtable call below); `kind`
//  is the stack argument tested against -1, `arg4`/`arg8`/`argC` the other
//  three stack arguments forwarded straight through to the vtable call - four
//  stack arguments in all, matching this function's own `retn 10h`.
CRenderDevice* __fastcall SwitchRenderDevice(void* recv0, int recv1,
                                              int kind, int arg4, int arg8, int argC)
{
    int reserved;
    if (kind == -1)
    {
        reserved = 0;
    }
    else
    {
        int* table = (int*)g_screenModeTable;
        reserved = table[kind * 6];    // three-int stride, times 8 bytes = *24
    }

    g_screenStatusB = recv1;
    g_screenStatusA = kind;

    CRenderDevice* device;
    if (kind == 0)
        device = (CRenderDevice*)GameMain_sub_6F52B1C0();
    else if (kind == 1)
        device = (CRenderDevice*)GameMain_sub_6F52A0C0();
    else
        device = g_renderDevice;

    g_renderDevice = device;

    typedef int (__thiscall *Slot0xCFn)(CRenderDevice*, int, int, int, int, int);
    int result = ((Slot0xCFn)(*(void***)device)[0xC / 4])(device, argC, arg8,
                                                            arg4, reserved, recv1);

    if (result != 0)
    {
        Storm_469((void*)&RenderDeviceResetContinuation);

        CallRenderDeviceSlot0x3C(0xFF000000);

        CallRenderDeviceSlot0x44(1);
        CallRenderDeviceSlot0x44(1);

        g_failedHeight = recv1;
        g_failedWidth = arg4;
        GameMain_sub_6F525F90(1);

        return g_renderDevice;
    }

    if (g_renderDevice != 0)
    {
        typedef void (__thiscall *Slot8Fn)(CRenderDevice*, int);
        ((Slot8Fn)(*(void***)g_renderDevice)[8 / 4])(g_renderDevice, 1);
    }
    return 0;
}
