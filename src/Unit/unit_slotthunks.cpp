//============================================================================
//  Pass-through definitions for the CUnit vtable slots this session did not
//  get to.  See docs/targets/CUnit__vtable.md for the worklist and
//  unit.h for the declarations.
//
//  They exist for one reason: MSVC emits ??_7CUnit@@6B@ with all 118 slot
//  addresses in it, so every slot needs a definition or nothing that
//  actually links the tree can build (LNK2019, thirty-one times over).
//  Each body jumps to the shipped function at its real address, so an
//  unreconstructed slot simply runs original code.
//
//  Same shape and same rules as unreconstructed_thunks.cpp: **not**
//  reconstructions, never offer one as hookable - a detour cannot detour a
//  detour - and delete each one as its real body lands.  Their argument
//  counts come from unit.h, which took them off each shipped body's own
//  `retn`, and tools/thunk_abi_audit.py checks every one of them against
//  its dump.
//============================================================================
#include "unit.h"
#include "CFloat.h"
#include "fvec3.h"

#define ADDR_THUNK_UNITSLOT(sig, addr)                \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//  slot 3 - real body now, unit_dispatch.cpp.

//  slot 15 - reconstructed; see unit_load.cpp.

//  slot 16
ADDR_THUNK_UNITSLOT(void CUnit::RefreshOwningPlayerColor(), 0x6F29E930)

//  slot 32 - real body now, unit_clearpendingnotify.cpp.

//  slot 42 - real body now, unit_flagbit8.cpp.

//  slot 45 - real body now, unit_ownershiphighlight.cpp
//  (CUnit::Method_0xB4).

//  slot 55

//  slot 57
ADDR_THUNK_UNITSLOT(float CUnit::Method_0xE4(FVec3* pos, int a1, int a2, int a3), 0x6F281F50)

//  slot 61 - real body now, unit_refreshspritevisibility.cpp.

//  slot 64 - real body now, unit_publishposition.cpp.

//  slot 70 - reconstructed; see unit_abilityoverridefloat.cpp.

//  slot 72
ADDR_THUNK_UNITSLOT(void* CUnit::NegateAndForwardOffset(void* target, struct SWidgetOffsetSource* src, int a3, int a4), 0x6F2A40D0)

//  slot 91
ADDR_THUNK_UNITSLOT(void CUnit::RefreshPlacementRadius(), 0x6F28D4F0)

//  slot 96 - real body now, unit_reposition.cpp.

//  slot 99
//  0x6F285650 CUnit::Method_0x18C is a real body now -
//  unit_playermasksearch.cpp.

//  slot 100 - real body now, unit_selectionhighlight.cpp.

//  slot 101
ADDR_THUNK_UNITSLOT(void CUnit::EnsureSelectionCircle(int wantUi, int runSlot103, int wantCircle, int unused3, int unused4), 0x6F28DCF0)

//  slot 103
ADDR_THUNK_UNITSLOT(int CUnit::Method_0x19C(), 0x6F27A730)

//  slot 104 - now a real body: unit_attachmentplacement.cpp.

//  slot 105
//  0x6F26F4B0 CUnit::SetAttachmentScale is a real body now -
//  unit_attachmentscale.cpp.

//  slot 107
ADDR_THUNK_UNITSLOT(int CUnit::Method_0x1AC(int a, int b, int c, int d), 0x6F2A0E30)

//  slot 110
ADDR_THUNK_UNITSLOT(int CUnit::Method_0x1B8(int a, int b), 0x6F282160)

//  slot 113 - 0x6F27A620 is a real body now - unit_footprintscaleoverride.cpp.

//  slot 114
//  0x6F2856C0 CUnit::Method_0x1C8 is a real body now -
//  unit_playermasksearch.cpp.

//  slot 115
ADDR_THUNK_UNITSLOT(int CUnit::Method_0x1CC(int a, int b), 0x6F285BF0)

//  slot 116
ADDR_THUNK_UNITSLOT(int CUnit::Method_0x1D0(int a, int b), 0x6F2979E0)

//  slot 117
ADDR_THUNK_UNITSLOT(int CUnit::Method_0x1D4(int a, int b), 0x6F297B30)

#undef ADDR_THUNK_UNITSLOT
