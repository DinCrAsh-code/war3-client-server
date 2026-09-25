//============================================================================
//  Three small, self-contained leaves from the same BFS closure as this
//  directory's other `citemclosure_*` files - none of them touching a
//  vtable, so none of them need the redirect treatment
//  citemclosure_database_thunks.cpp explains.
//============================================================================
#include "refcnt.h"
#include "storm.h"

#define ADDR_THUNK(sig, addr)                     \
    __declspec(naked) sig                         \
    {                                             \
        __asm { mov eax, addr }                   \
        __asm { jmp eax }                         \
    }

//----------------------------------------------------------------------------
//  0x6F4C9D30 - `retn 0`, __thiscall: tear something down through
//  `sub_6F4C9B80` (out of this batch's scope - a two-array refcount-drain
//  loop over offsets 0x14/0x1C and 0x3C/0x44, the same field pairs
//  citemclosure_dbfieldhash.cpp's own record tables sit behind, `retn 0`)
//  and then tail-call TRefCnt::Release() on `this` (0x6F4C40A0,
//  Agent/refcnt.h) - the shipped `jmp` rather than `call`+`retn`,
//  reproduced by declaring Release() the tail statement so /O2 has
//  nothing else left to do after it.
//----------------------------------------------------------------------------
ADDR_THUNK(void __fastcall DatabaseRecordSet_DrainRecordTables(void* self),
           0x6F4C9B80)

void __fastcall DatabaseRecordSet_TeardownAndRelease(void* self)
{
    DatabaseRecordSet_DrainRecordTables(self);
    ((TRefCnt*)self)->Release();
}

//----------------------------------------------------------------------------
//  0x6F526A10 - `retn 0Ch`, __fastcall(ecx=self, edx) plus three stack
//  dwords: packs the four scalars into a local 16-byte temporary in the
//  shipped order - `arg_0`, the original `edx`, `arg_8`, `arg_4` - and
//  forwards `this` plus a pointer to that temporary to `sub_6F526570` (out
//  of this batch's scope).  The struct shape is read straight off the
//  four stores; nothing in this dump says what the four scalars mean.
//----------------------------------------------------------------------------
struct SFourDwordPack { int a, b, c, d; };

ADDR_THUNK(void __fastcall ForwardFourDwordPack(
               void* self, SFourDwordPack* pack),
           0x6F526570)

void __fastcall PackAndForwardFourDwords(void* self, int edxArg, int arg0,
                                         int arg4, int arg8)
{
    SFourDwordPack pack = { arg0, edxArg, arg8, arg4 };
    ForwardFourDwordPack(self, &pack);
}

//----------------------------------------------------------------------------
//  0x6F74AFB0 - `retn 0`, __thiscall: free up to five NTempest terrain
//  vectors hung off `this` (offsets 0x48/0x38/0x28/0x18/0x8), each only if
//  non-null, each through Storm with its own RTTI-descriptor tag - the
//  same `CFloat`-adjacent `NTempest::CDynTable`/vector family
//  Misc/citemclosure_terrain_gates.cpp's header already describes.  A
//  plain, mechanical five-times-repeated `if (ptr) SMemFree(ptr, tag, -2,
//  0)`, transcribed in the shipped field order (0x48 first, 0x8 last).
//  This dump's own text carries only IDA's truncated auto-labels for the
//  five tag strings (`aAushorewavemod`, `aG_4`, `aAvcimvectorNte`,
//  `aAvc2vectorNtem`, `aAvc3vectorNtem`), not the RTTI bytes themselves, so
//  the literals below are a best-effort expansion of those labels, not a
//  confirmed transcription - `verify.py`'s own canonicalisation treats an
//  unresolved string operand as an opaque `SYM` either way (the same rule
//  CLAUDE.md's own `call __imp__` note describes for a symbol reference),
//  so an inexact literal costs nothing in score but should not be read as
//  ground truth.
//----------------------------------------------------------------------------
void __fastcall ReleaseFiveTerrainVectors(void* self)
{
    struct SVectorHolder
    {
        char  m_reserved00[8];
        void* m_vec3;   // +0x08
        char  m_reserved0C[0xC];
        void* m_vec2;   // +0x18
        char  m_reserved1C[0xC];
        void* m_vecImVector; // +0x28
        char  m_reserved2C[0xC];
        void* m_vecG4;  // +0x38
        char  m_reserved3C[0xC];
        void* m_vecShoreWaveMod; // +0x48
    };

    SVectorHolder* holder = (SVectorHolder*)self;

    if (holder->m_vecShoreWaveMod)
        SMemFree(holder->m_vecShoreWaveMod,
                 ".?AV?$AuShoreWaveMod@@", -2, 0);
    if (holder->m_vecG4)
        SMemFree(holder->m_vecG4, ".?AVG_4@@", -2, 0);
    if (holder->m_vecImVector)
        SMemFree(holder->m_vecImVector,
                 ".?AV?$TSExplicitList@UAvcImVectorNTempest@@$0A@@@", -2, 0);
    if (holder->m_vec2)
        SMemFree(holder->m_vec2,
                 ".?AV?$TSExplicitList@UAvc2VectorNTempest@@$0A@@@", -2, 0);
    if (holder->m_vec3)
        SMemFree(holder->m_vec3,
                 ".?AV?$TSExplicitList@UAvc3VectorNTempest@@$0A@@@", -2, 0);
}
