//============================================================================
//  See abilitycustomdata.h for why both of these stay redirects.  Spelled
//  as `Construct` members rather than real constructors for the reason
//  Containers/hashtablectors.cpp already documents: MSVC will not take
//  __declspec(naked) on a constructor.  Each hands `this` back, matching
//  the shipped `mov eax, esi` / `retn` (CCustomData::Construct) or
//  `mov eax, esi` / `retn` (CAbilityCustomData::Construct) before their
//  own returns.
//============================================================================
#include "abilitycustomdata.h"

#define CTOR_THUNK(addr)         \
    {                            \
        __asm { mov eax, addr }  \
        __asm { jmp eax }        \
    }

//  0x6F2518A0 - `retn 4`.  ecx = this (pre-allocated storage), one stack
//  arg (the type tag 0-6).
__declspec(naked) CCustomData* CCustomData::Construct(int type)
    CTOR_THUNK(06F2518A0h)

//  0x6F256520 - `retn 0`.  ecx = this (pre-allocated storage), no stack
//  args - calls CCustomData::Construct(4) itself.
__declspec(naked) CAbilityCustomData* CAbilityCustomData::Construct()
    CTOR_THUNK(06F256520h)

#undef CTOR_THUNK
