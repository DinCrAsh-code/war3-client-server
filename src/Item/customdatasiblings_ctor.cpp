//============================================================================
//  See customdatasiblings.h for why all six of these stay redirects -
//  identical reasoning to CAbilityCustomData::Construct
//  (abilitycustomdata_ctor.cpp): each calls the SEH-carrying
//  `CCustomData::Construct(int)` with its own type tag, then stamps its
//  own vtable over the base's.
//============================================================================
#include "customdatasiblings.h"

#define CTOR_THUNK(addr)         \
    {                            \
        __asm { mov eax, addr }  \
        __asm { jmp eax }        \
    }

//  0x6F2564C0 - `retn 0`, type tag 0.
__declspec(naked) CUnitCustomData* CUnitCustomData::Construct()
    CTOR_THUNK(06F2564C0h)

//  0x6F2564F0 - `retn 0`, type tag 1.
__declspec(naked) CItemCustomData* CItemCustomData::Construct()
    CTOR_THUNK(06F2564F0h)

//  0x6F2565B0 - `retn 0`, type tag 2.
__declspec(naked) CDestructableCustomData* CDestructableCustomData::Construct()
    CTOR_THUNK(06F2565B0h)

//  0x6F2565E0 - `retn 0`, type tag 3.
__declspec(naked) CDoodadCustomData* CDoodadCustomData::Construct()
    CTOR_THUNK(06F2565E0h)

//  0x6F256550 - `retn 0`, type tag 5.
__declspec(naked) CBuffCustomData* CBuffCustomData::Construct()
    CTOR_THUNK(06F256550h)

//  0x6F256580 - `retn 0`, type tag 6.
__declspec(naked) CUpgradeCustomData* CUpgradeCustomData::Construct()
    CTOR_THUNK(06F256580h)

#undef CTOR_THUNK
