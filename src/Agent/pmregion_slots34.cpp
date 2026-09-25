//============================================================================
//  NIpse::CPmRegion's own vtable slots 3/4 (0x6F49E0C0/0x6F49E0D0) - bare
//  `jmp` forwards into NTempest::CPresenceTagged's own already-reconstructed
//  RegisterOrReuse/UnregisterAndNotify (Pathfinding/handletable.h). See
//  pmregion.h's own declaration comments for why these are free functions,
//  not CPmRegion members, and why a naked `jmp` to the qualified C++ name
//  rather than a raw address (docs/msvc-vc8-idioms.md's own "An
//  MSVC-generated adjustor thunk into an already-reconstructed function").
//============================================================================
#include "pmregion.h"

namespace NIpse {

//  0x6F49E0C0 - vtable slot 3 (+0x0C).
__declspec(naked) void __fastcall PmRegion_InitGrid(void* /*thisPtr*/)
{
    __asm { jmp SHandleTableEntry::RegisterOrReuse }
}

//  0x6F49E0D0 - vtable slot 4 (+0x10).
__declspec(naked) void __fastcall PmRegion_Teardown(void* /*thisPtr*/)
{
    __asm { jmp SHandleTableEntry::UnregisterAndNotify }
}

}  // namespace NIpse
