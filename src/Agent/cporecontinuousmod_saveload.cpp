//============================================================================
//  0x6F4A8EC0 / 0x6F4A8EF0 - NIpse::CPoReContinuousMod::Save/Load, vtable
//  slots 9/10. CAgentRelation::Save/Load first, then m_rate/m_duration via
//  WriteCFloat/ReadCFloat, chained through the previous call's own
//  returned pointer.
//
//  Written naked: plain C++ chaining scored 0.788/18 - the shipped code
//  caches `&m_duration` in a callee-saved register (ebx) across the
//  m_rate call rather than recomputing it from `this` afterwards, the
//  same register-allocation-only gap Agent/cpolink_saveload.cpp's own
//  Load hit.
//============================================================================
#include "cporecontinuousmod.h"
#include "agentrelation.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"

CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

namespace NIpse {

__declspec(naked) void CPoReContinuousMod::Save(void* /*writer*/)
{
    __asm
    {
        push    ebx
        push    esi
        push    edi
        mov     edi, [esp+10h]
        push    edi
        mov     esi, ecx
        call    CAgentRelation::Save
        lea     edx, [esi+50h]
        mov     ecx, edi
        lea     ebx, [esi+54h]
        call    WriteCFloat
        mov     edx, ebx
        mov     ecx, eax
        call    WriteCFloat
        pop     edi
        pop     esi
        pop     ebx
        retn    4
    }
}

__declspec(naked) void CPoReContinuousMod::Load(void* /*reader*/)
{
    __asm
    {
        push    ebx
        push    esi
        push    edi
        mov     edi, [esp+10h]
        push    edi
        mov     esi, ecx
        call    CAgentRelation::Load
        lea     edx, [esi+50h]
        mov     ecx, edi
        lea     ebx, [esi+54h]
        call    ReadCFloat
        mov     edx, ebx
        mov     ecx, eax
        call    ReadCFloat
        pop     edi
        pop     esi
        pop     ebx
        retn    4
    }
}

}  // namespace NIpse
