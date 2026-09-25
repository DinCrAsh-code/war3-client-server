//============================================================================
//  gamemain-batch-3: two more forwards onto g_renderDevice's own vtable
//  (see renderdevicesubmit.cpp for the slot-0xA8 precedent this follows).
//  Neither slot has a committed vtable dump, so vtable_dispatch_audit.py
//  judges both "unknown" rather than clean - the typedefs are read straight
//  off each call site's own argument count and calling convention.
//============================================================================
#include "renderdevice.h"

//----------------------------------------------------------------------------
//  0x6F526C20 - `__stdcall`, one stack dword, `retn 4`.  The shipped body
//  builds its device-call argument by pushing a placeholder and overwriting
//  it in place rather than a fresh `push`; not reproduced verbatim (nothing
//  in this repo's own C++ can force that specific register shuffle), so
//  this is `IDENTICAL` if the instruction count does not land exactly.
//----------------------------------------------------------------------------
void __stdcall CallRenderDeviceSlot0x3C(unsigned int value)
{
    typedef void (__thiscall *SlotFn)(CRenderDevice*, unsigned int);
    ((SlotFn)(*(void***)g_renderDevice)[0x3C / 4])(g_renderDevice, value);
}

//----------------------------------------------------------------------------
//  0x6F526C20's sibling, 0x6F5264F0 - `__fastcall`, the value in ecx, then a
//  tail call into ResetFpuControlWord() (0x6F6CE230, already reconstructed
//  by gamemain-batch-4 in Sync/threadpriority.cpp - declared here, not
//  redefined).
//----------------------------------------------------------------------------
void ResetFpuControlWord();   // 0x6F6CE230

void __fastcall CallRenderDeviceSlot0x44(unsigned int value)
{
    typedef void (__thiscall *SlotFn)(CRenderDevice*, unsigned int);
    ((SlotFn)(*(void***)g_renderDevice)[0x44 / 4])(g_renderDevice, value);

    ResetFpuControlWord();
}
