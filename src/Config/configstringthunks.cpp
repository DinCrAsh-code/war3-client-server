//============================================================================
//  A redirect for the configuration string reader.  0x6F00A040 is below
//  this dump's depth cut - nothing here reconstructs it - so the body jumps
//  to the shipped function and the real, unhooked code runs.
//
//  NOT a reconstruction.  Its `retn 0Ch` is fixed by its call sites: both
//  push three dwords and neither adjusts esp afterwards.
//============================================================================

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

ADDR_THUNK(int __fastcall GetConfigStringValue(const char* section,
                                               const char* key, char* buffer,
                                               unsigned int size, int flags),
           0x6F00A040)

#undef ADDR_THUNK
