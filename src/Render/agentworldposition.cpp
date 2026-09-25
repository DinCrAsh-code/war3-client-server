//============================================================================
//  0x6F4D3100 - the agent's world position.
//
//  Vtable slot 0x3C returns the vector *by value*: the caller hands down a
//  buffer as the one stack argument and the slot answers it in eax, which is
//  why the body has a twelve-byte local it never reads and copies out of
//  eax instead.
//============================================================================
#include "spriteattach.h"

void __fastcall GetAgentWorldPosition(void* agent, float* out)
{
    C3VECTOR buffer;

    typedef C3VECTOR* (__thiscall *GetPositionFn)(void*, C3VECTOR*);
    const C3VECTOR* p =
        ((GetPositionFn)(*(void***)agent)[0x3C / 4])(agent, &buffer);

    *(C3VECTOR*)out = *p;
}
