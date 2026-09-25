//============================================================================
//  Leaves CGameWar3::ComputeChecksum (vtable slot 0) and
//  CGameWar3::ReleaseIfFlagged (vtable slot 2) call into but this batch did
//  not reconstruct - out of scope for the checksum-provider identification
//  itself.  `retn` values read off each one's own body.
//============================================================================
extern "C" {

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                              \
        __asm mov eax, addr                       \
        __asm jmp eax                              \
    }

//  0x6F406CF0 - retn 0.  m_field34's own vtable slot 0.
ADDR_THUNK(unsigned int __fastcall Field34ChecksumHash(void*), 0x6F406CF0)

//  0x6F3A7E00 - retn 0.  264 instructions, its own SEH frame; called for
//  side effect only by CGameWar3::ReleaseIfFlagged, which never reads its
//  return value.
ADDR_THUNK(void __fastcall CGameWar3_SideEffect7E00(void*), 0x6F3A7E00)

}
