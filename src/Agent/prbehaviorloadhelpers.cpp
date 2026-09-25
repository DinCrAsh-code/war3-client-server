//============================================================================
//  Two naked pass-through thunks for CPrBehavior::Load's own callees
//  (prbehavior_load.cpp) that no session has reconstructed yet.
//
//  Same shape and same reasoning as Pathfinding/pathmove_thunks.cpp: jump to
//  the shipped function at the address the declaration's own comment
//  records, so the original code runs. These are NOT reconstructions and
//  must never be offered as hookable - `abi_audit.py` sees them as
//  SELF-REDIRECT THUNKs and `generate_mix_config.py` defaults every new key
//  to false; leave them false. Delete each one as its real body lands.
//
//  0x6F4AA720 in particular is a genuinely deep, genuinely new "allocate
//  and load one SSyncRegistration-shaped CSIRequest object" leaf (RTTI type
//  descriptor string "aAvCsirequestVc_0" seen inside its own callee,
//  sub_6F4AA5A0, retn 0x14) - reaching it for real means reconstructing an
//  entirely new allocator pattern this cluster's own ten TODO addresses
//  never asked for, so it stays a redirect rather than growing this
//  session's scope past what CPrBehavior::Load itself needs to compile and
//  match. 0x6F483290 is a smaller, six-instruction leaf (a two-way pointer
//  select into g_pTimeSync keyed on this object's own handle-pool bit) with
//  the same "not part of this cluster's own ten addresses" reasoning.
//============================================================================

//  0x6F483290 - shipped code. `ecx` is the fixed CTimeSync* global
//  (dword_6FAB73D8), the one stack dword is `this` (a CPrBehavior*, tested
//  for its own +0x14 handle-pool bit). `retn 4`.
struct STimeSyncPoolSelector
{
    void* SelectSubsystem(void* self);
};

__declspec(naked) void* STimeSyncPoolSelector::SelectSubsystem(void* /*self*/)
{
    __asm { mov eax, 06F483290h
            jmp eax }
}

//  0x6F4AA720 - shipped code. `__fastcall`: `store` in ecx, the selected
//  subsystem pointer (0x6F483290's own return) in edx, `this` as the one
//  stack dword. `retn 4`. Returns the freshly loaded/allocated
//  SSyncRegistration-shaped object.
extern "C" __declspec(naked) void* __fastcall LoadSyncRegistrationFromStore(void* /*store*/, void* /*subsystem*/, void* /*self*/)
{
    __asm { mov eax, 06F4AA720h
            jmp eax }
}
