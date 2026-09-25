//============================================================================
//  Four addresses from the jscript-bin4 closure (the ".j" script-reader
//  round), each kept as a documented naked redirect rather than a
//  reconstruction - not out-of-scope, just genuinely blocked this session:
//
//  - CItemDB::CItemDB / CUnitDB::CUnitDB (0x6F70FB20/0x6F70FC90) both open
//    with the game's own __except_handler4-shaped SEH frame
//    (`push -1` / `push offset SEH_...` / `mov eax, large fs:0`) this fixed
//    `/GS- /EHs-c-` toolchain cannot emit - the same ceiling
//    pipeline/CLAUDE.md and docs/msvc-vc8-idioms.md already document for
//    every other SEH-framed function in this binary.  Both real vtable
//    symbols (`??_7CItemDB@@6B@`/`??_7CUnitDB@@6B@`) are read straight out
//    of their own dumps, so the names are real even though the bodies are
//    not reconstructed; both take `this` in `ecx` with no stack argument
//    (`retn 0`).
//  - 0x6F71C530 - a lazy-init singleton (`dword_6FADB0F4`) also opens with
//    the same SEH frame, over a `Storm_401`-allocated sound-path buffer.
//    Free function, no arguments, `retn 0`; no evidence for a real name
//    beyond what it does.
//  - 0x6F7E1876 - a plain CRT internal: rounds `eax` up to the next
//    multiple of 8 before falling straight into `__alloca_probe`
//    (`jmp __alloca_probe`, the compiler-generated large-stack-allocation
//    probe).  Not part of any target's own logic; kept for whatever future
//    body ends up needing an 8-byte-aligned variant of the probe.
//============================================================================
#include "storm.h"

class CItemDB
{
public:
    //  0x6F70FB20 - SEH-framed (see file header); `retn 0`.
    CItemDB();
};

class CUnitDB
{
public:
    //  0x6F70FC90 - SEH-framed (see file header); `retn 0`.
    CUnitDB();
};

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

ADDR_THUNK(CItemDB::CItemDB(), 0x6F70FB20)
ADDR_THUNK(CUnitDB::CUnitDB(), 0x6F70FC90)

//  0x6F71C530 - free function, no arguments, `retn 0`.
ADDR_THUNK(void InitLazySoundPathSingleton(), 0x6F71C530)

//  0x6F7E1876 - `__alloca_probe_8`-shaped: round `eax` up to a multiple of
//  8 and tail into `__alloca_probe`.  No declared arguments - this is
//  reached purely through the `eax`/no-stack-args convention the compiler
//  itself uses for its own alloca probes, never through an ordinary call.
ADDR_THUNK(void AllocaProbe8Aligned(), 0x6F7E1876)

#undef ADDR_THUNK
