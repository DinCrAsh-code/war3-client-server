//============================================================================
//  citem-batch6: four more forwards onto g_renderDevice - three more virtual
//  slots (0x18, 0x1C, 0x20 - the render device's own vtable pointer is its
//  first field, same "the singleton is polymorphic" shape
//  renderdeviceslot24thunk.cpp/renderdeviceslot8c.cpp/renderdeviceslot3c44.cpp
//  already forward through) and one plain field read.  All four are reached
//  only from CPreferencesWar3::ApplyPreference (Config/preferenceapply.cpp,
//  cases 9/5/10/10 respectively) - graphics-option setters/getters this
//  closure never names beyond "the device's own slot N".
//
//  Written naked rather than through the typedef-call idiom the other
//  renderdeviceslotNN.cpp files use: each is a handful of instructions with
//  a specific register shuffle (the incoming `ecx` value saved to `eax`
//  before `ecx` is reloaded with the device, or the float round-tripped
//  through a freshly reserved stack slot rather than pushed directly), and
//  transcribing the bytes exactly removes any risk of the compiler picking
//  a different register plan for an equivalent-looking C++ spelling.
//============================================================================
#include "renderdevice.h"

//----------------------------------------------------------------------------
//  0x6F525EA0 - slot 0x18.  `__fastcall`, one value in ecx, `retn 0`: the
//  value is pushed as the device call's own one stack argument, with the
//  device left in ecx for that (thiscall) call.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall RenderDeviceSlot0x18Forward(unsigned int)
{
    __asm
    {
        mov     eax, ecx
        mov     ecx, g_renderDevice
        mov     edx, [ecx]
        push    eax
        mov     eax, [edx + 18h]
        call    eax
        retn
    }
}

//----------------------------------------------------------------------------
//  0x6F525EC0 - slot 0x1C.  One stack float in, `retn 4`.  The shipped body
//  reserves the device call's argument slots with `push 0` / `push ecx`
//  and then overwrites the first with the float through `fstp` rather than
//  pushing it directly - the same round-trip renderdeviceslot3c44.cpp notes
//  for slot 0x3C, reproduced here rather than argued about.
//----------------------------------------------------------------------------
__declspec(naked) void __stdcall RenderDeviceSlot0x1CForward(float)
{
    __asm
    {
        mov     ecx, g_renderDevice
        fld     dword ptr [esp + 4]
        mov     eax, [ecx]
        mov     edx, [eax + 1Ch]
        push    0
        push    ecx
        fstp    dword ptr [esp]
        call    edx
        retn    4
    }
}

//----------------------------------------------------------------------------
//  0x6F525EE0 - slot 0x20.  Same shape as slot 0x18 above.
//----------------------------------------------------------------------------
__declspec(naked) void __fastcall RenderDeviceSlot0x20Forward(unsigned int)
{
    __asm
    {
        mov     eax, ecx
        mov     ecx, g_renderDevice
        mov     edx, [ecx]
        push    eax
        mov     eax, [edx + 20h]
        call    eax
        retn
    }
}

//----------------------------------------------------------------------------
//  0x6F525F30 - a plain field read, not a vtable call: the device's own
//  +0xDC dword (still unnamed - nothing else in this closure reads it).
//  IDA mis-resolved the tail `jmp` as a Concurrency library symbol that
//  happens to share its address; the real target
//  (`mov eax,[ecx+0DCh] / retn`) is folded straight into this one function.
//----------------------------------------------------------------------------
__declspec(naked) unsigned int GetRenderDeviceField0xDC()
{
    __asm
    {
        mov     ecx, g_renderDevice
        jmp     l_tail
l_tail:
        mov     eax, [ecx + 0DCh]
        retn
    }
}
