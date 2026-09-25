//============================================================================
//  0x6F4D31B0 - InvokeControllerRampVTableSlot0x28.  See
//  cameracontrollerbind.h for why this stays a literal naked reproduction
//  rather than a typed vtable dispatch.
//============================================================================
#include "cameracontrollerbind.h"

__declspec(naked) void __fastcall InvokeControllerRampVTableSlot0x28(void*, float)
{
    __asm
    {
        mov     eax, [ecx]
        fld     dword ptr [esp+4]
        mov     edx, [eax+28h]
        push    ecx
        fstp    dword ptr [esp]
        call    edx
        retn    4
    }
}
