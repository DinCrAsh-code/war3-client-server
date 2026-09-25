//============================================================================
//  0x6F4A9AA0 / 0x6F4A9AD0 - NIpse::CPoReThresholdLis::Save/Load, vtable
//  slots 9/10. CAgentRelation::Save/Load first, then m_threshold
//  (WriteCFloat/ReadCFloat) and m_mode (WriteDwordAlt2/ReadDwordAlt3),
//  chained through the previous call's own returned pointer.
//
//  Written naked: plain C++ chaining scored 0.606/0.824 - the shipped
//  code caches m_mode (Save) in a callee-saved register across the
//  WriteCFloat call, and Load's own ReadDwordAlt3 result is stored
//  through a stack round-trip rather than directly, neither of which a
//  straightforward sequential rewrite reproduces.
//============================================================================
#include "cporethresholdlis.h"
#include "agentrelation.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"

CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

namespace NIpse {

__declspec(naked) void CPoReThresholdLis::Save(void* /*writer*/)
{
    __asm
    {
        push    ebx
        push    esi
        push    edi
        mov     edi, [esp+0Ch]
        push    edi
        mov     esi, ecx
        call    CAgentRelation::Save
        mov     ebx, [esi+54h]
        lea     edx, [esi+50h]
        mov     ecx, edi
        call    WriteCFloat
        push    ebx
        mov     ecx, eax
        call    CDataStoreScratch::WriteDwordAlt2
        pop     edi
        pop     esi
        pop     ebx
        retn    4
    }
}

__declspec(naked) void CPoReThresholdLis::Load(void* /*reader*/)
{
    __asm
    {
        push    esi
        push    edi
        mov     edi, [esp+0Ch]
        push    edi
        mov     esi, ecx
        call    CAgentRelation::Load
        lea     edx, [esi+50h]
        mov     ecx, edi
        call    ReadCFloat
        lea     ecx, [esp+0Ch]
        push    ecx
        mov     ecx, eax
        call    CDataStore::ReadDwordAlt3
        mov     edx, [esp+0Ch]
        pop     edi
        mov     [esi+54h], edx
        pop     esi
        retn    4
    }
}

}  // namespace NIpse
