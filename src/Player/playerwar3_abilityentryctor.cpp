//============================================================================
//  0x6F208450 - CPlayerWar3::AbilityEntry::AbilityEntry().  See playerwar3.h
//  for why this is a redirect rather than a reconstruction: no dump in this
//  session's scope carries this address's own body, only its address as a
//  function pointer.  `retn 0` per the `void (__thiscall *)(void *)`
//  callback type the `eh vector constructor iterator` call in
//  CPlayerWar3::CPlayerWar3() (playerwar3_ctor.cpp) declares it with.
//============================================================================
#include "playerwar3.h"

__declspec(naked) void CPlayerWar3::AbilityEntry::Construct()
{
    __asm
    {
        mov     eax, 06F208450h
        jmp     eax
    }
}
