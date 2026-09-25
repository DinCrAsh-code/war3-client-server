//============================================================================
//  0x6F4A7ED0 / 0x6F4A7F10 - NIpse::CPoPoInterfLis::Save/Load, vtable
//  slots 9/10. CAgentRelation::Save/Load first, then m_radius (WriteCFloat/
//  ReadCFloat) and m_opt (WriteDwordAlt2/ReadDwordAlt3), then the +0x58
//  array's own header + every live element
//  (WriteInterfLisEntries/ReadInterfLisEntries, cpopointerflis_array.cpp).
//
//  Written naked - same register-caching-across-calls reasoning
//  Agent/cpopovelocitymod_saveload.cpp's own header documents for its
//  sibling: the shipped stream re-assigns the cached writer/reader pointer
//  (edi) to WriteCFloat's/ReadCFloat's own return value after the first
//  field, then keeps reusing it unchanged for the rest of the calls,
//  rather than re-loading the argument fresh each time.
//============================================================================
#include "cpopointerflis.h"
#include "agentrelation.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"

CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

namespace NIpse {

__declspec(naked) void CPoPoInterfLis::Save(void* /*writer*/)
{
    __asm
    {
        push    esi
        push    edi
        mov     edi, [esp+0Ch]
        push    edi
        mov     esi, ecx
        call    CAgentRelation::Save
        lea     edx, [esi+50h]
        mov     ecx, edi
        call    WriteCFloat
        mov     edi, eax
        mov     eax, [esi+54h]
        push    eax
        mov     ecx, edi
        call    CDataStoreScratch::WriteDwordAlt2
        lea     edx, [esi+58h]
        mov     ecx, edi
        call    WriteInterfLisEntries
        pop     edi
        pop     esi
        retn    4
    }
}

__declspec(naked) void CPoPoInterfLis::Load(void* /*reader*/)
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
        mov     edi, eax
        lea     eax, [esi+54h]
        push    eax
        mov     ecx, edi
        call    CDataStore::ReadDwordAlt3
        lea     edx, [esi+58h]
        mov     ecx, edi
        call    ReadInterfLisEntries
        pop     edi
        pop     esi
        retn    4
    }
}

}  // namespace NIpse
