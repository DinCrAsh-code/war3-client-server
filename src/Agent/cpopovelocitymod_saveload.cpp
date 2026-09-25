//============================================================================
//  0x6F4A7850 / 0x6F4A7890 - NIpse::CPoPoVelocityMod::Save/Load, vtable
//  slots 9/10. CAgentRelation::Save/Load first, then m_velocityX,
//  m_velocityY, m_duration in that order via WriteCFloat/ReadCFloat.
//
//  Written naked - same register-caching-across-calls reasoning
//  Agent/cporecontinuousmod_saveload.cpp's own header documents for its
//  two-field sibling; this one carries a third field so caches TWO
//  addresses (ebx/edi) across the chained calls instead of one.
//============================================================================
#include "cpopovelocitymod.h"
#include "agentrelation.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"

CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

namespace NIpse {

__declspec(naked) void CPoPoVelocityMod::Save(void* /*writer*/)
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
        lea     ebx, [esi+50h]
        mov     edx, ebx
        mov     ecx, edi
        call    WriteCFloat
        lea     edx, [ebx+4]
        mov     ecx, edi
        call    WriteCFloat
        lea     edx, [esi+58h]
        mov     ecx, edi
        call    WriteCFloat
        pop     edi
        pop     esi
        pop     ebx
        retn    4
    }
}

__declspec(naked) void CPoPoVelocityMod::Load(void* /*reader*/)
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
        lea     ebx, [esi+50h]
        mov     edx, ebx
        mov     ecx, edi
        call    ReadCFloat
        lea     edx, [ebx+4]
        mov     ecx, edi
        call    ReadCFloat
        lea     edx, [esi+58h]
        mov     ecx, edi
        call    ReadCFloat
        pop     edi
        pop     esi
        pop     ebx
        retn    4
    }
}

}  // namespace NIpse
