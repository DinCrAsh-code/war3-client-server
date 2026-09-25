//============================================================================
//  0x6F4A97C0 / 0x6F4A9810 - NIpse::CPoRePulseMod::Save/Load, vtable slots
//  9/10. CAgentRelation::Save/Load first, then m_interval, m_frequency
//  (WriteCFloat/ReadCFloat), m_opt, m_pulseCount (plain WriteDword/
//  ReadDword), each chained through the previous call's own returned
//  pointer.
//
//  Written naked: plain C++ chaining scored 0.653/0.857 - the shipped
//  code caches m_opt (Save) / advances a running field pointer (Load) in
//  a callee-saved register across the intervening calls rather than
//  recomputing the address from `this` each time, the same register-
//  allocation-only gap Agent/cpolink_saveload.cpp's own Load hit.
//============================================================================
#include "cporepulsemod.h"
#include "agentrelation.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"

CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

namespace NIpse {

__declspec(naked) void CPoRePulseMod::Save(void* /*writer*/)
{
    __asm
    {
        push    ebx
        push    ebp
        push    esi
        push    edi
        mov     edi, [esp+14h]
        push    edi
        mov     esi, ecx
        call    CAgentRelation::Save
        mov     ebx, [esi+58h]
        lea     edx, [esi+50h]
        mov     ecx, edi
        lea     ebp, [esi+54h]
        call    WriteCFloat
        mov     edx, ebp
        mov     ecx, eax
        call    WriteCFloat
        push    ebx
        mov     ecx, eax
        call    CDataStoreScratch::WriteDword
        mov     ecx, [esi+5Ch]
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        mov     [esp+4], ecx
        mov     ecx, eax
        jmp     CDataStoreScratch::WriteDword
    }
}

__declspec(naked) void CPoRePulseMod::Load(void* /*reader*/)
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
        lea     ecx, [esi+58h]
        push    ecx
        mov     ecx, eax
        call    CDataStore::ReadDword
        add     esi, 5Ch
        push    esi
        mov     ecx, eax
        call    CDataStore::ReadDword
        pop     edi
        pop     esi
        pop     ebx
        retn    4
    }
}

}  // namespace NIpse
