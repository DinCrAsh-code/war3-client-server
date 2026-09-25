//============================================================================
//  CGameWar3's own sub-object constructors this batch dumped but did not
//  reconstruct - each is its own real __CxxFrameHandler3-shaped SEH frame
//  (43-78 instructions), and reconstructing all five in the same pass as
//  CGameWar3's own constructor was more than this session's budget.
//  Dumps are already in asm/ (this session's ida_query calls); each
//  `retn` below is read off its own body per thunk_abi_audit.py's rule.
//  A follow-up session's next step for this class - see
//  desync_mechanism_worklist.json batch B.
//============================================================================
#include "gamewar3.h"

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                              \
        __asm mov eax, addr                       \
        __asm jmp eax                              \
    }

//  0x6F3A18B0 - retn 4.  Zeroes the field then calls the '+gam'
//  (SCheckedGameSlot-family) registration helper 0x6F39F800 on it with the
//  candidate argument (always 0 from CGameWar3's own constructor).
ADDR_THUNK(void* __fastcall CGameWar3_ConstructField1C(void*, int), 0x6F3A18B0)

//  0x6F3AE870 - retn 0.  Stamps
//  ??_7?$TAllocatorHashTable@UIdStringReg@@VHASHKEY_4CC@@$0BAA@@@6B@ twice
//  (at its own +0x00 and +0x11C) into the 0x144-byte block CGameWar3's own
//  constructor allocates for it, and returns `this`.
ADDR_THUNK(void* __fastcall CGameWar3_ConstructField20(void*), 0x6F3AE870)

//  0x6F3A0D80 - retn 0.  Constructs the 0x64-byte block.
ADDR_THUNK(void* __fastcall CGameWar3_ConstructField30(void*), 0x6F3A0D80)

//  0x6F408C00 - retn 0.  Constructs the 0x8C-byte block.
ADDR_THUNK(void* __fastcall CGameWar3_ConstructField34(void*), 0x6F408C00)

//  0x6F3A1910 - retn 4.  Same shape as CGameWar3_ConstructField1C, on the
//  +0x400 field.
ADDR_THUNK(void* __fastcall CGameWar3_ConstructField400(void*, int), 0x6F3A1910)
