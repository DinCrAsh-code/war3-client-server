//============================================================================
//  0x6F4A9470 / 0x6F4A94C0 - NIpse::CPoReal::Save/Load, vtable slots 9/10.
//  CRlProperty::Save/Load first, then this class's own four CFloats
//  (+0x78/+0x7C/+0x80/+0x84, in that offset order) via WriteCFloat/
//  ReadCFloat, each chained through the previous call's own returned
//  store/reader pointer.
//
//  Written naked: plain C++ (chained via `s = WriteCFloat(s, &field);`)
//  scored 0.531/28 - the compiler kept `this` in a register throughout
//  and recomputed each field address from it, rather than caching two of
//  the four field addresses (`edi`/`ebp`) once up front and only chaining
//  the reader/writer pointer through eax the way the shipped code does.
//============================================================================
#include "cporeal.h"
#include "rlproperty.h"
#include "cdatastorescratch.h"
#include "cdatastore.h"

CDataStoreScratch* __fastcall WriteCFloat(CDataStoreScratch* store, const CFloat* value);
CDataStore* __fastcall ReadCFloat(CDataStore* store, CFloat* out);

namespace NIpse {

__declspec(naked) void CPoReal::Save(void* /*writer*/)
{
    __asm
    {
        push    ebx
        mov     ebx, [esp+8]
        push    ebp
        push    esi
        push    edi
        push    ebx
        mov     esi, ecx
        call    CRlProperty::Save
        lea     edx, [esi+78h]
        mov     ecx, ebx
        lea     edi, [esi+80h]
        lea     ebp, [esi+7Ch]
        call    WriteCFloat
        mov     edx, ebp
        mov     ecx, eax
        call    WriteCFloat
        mov     esi, eax
        mov     edx, edi
        mov     ecx, esi
        call    WriteCFloat
        lea     edx, [edi+4]
        mov     ecx, esi
        call    WriteCFloat
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        retn    4
    }
}

__declspec(naked) void CPoReal::Load(void* /*reader*/)
{
    __asm
    {
        push    ebx
        mov     ebx, [esp+8]
        push    ebp
        push    esi
        push    edi
        push    ebx
        mov     esi, ecx
        call    CRlProperty::Load
        lea     edx, [esi+78h]
        mov     ecx, ebx
        lea     edi, [esi+80h]
        lea     ebp, [esi+7Ch]
        call    ReadCFloat
        mov     edx, ebp
        mov     ecx, eax
        call    ReadCFloat
        mov     esi, eax
        mov     edx, edi
        mov     ecx, esi
        call    ReadCFloat
        lea     edx, [edi+4]
        mov     ecx, esi
        call    ReadCFloat
        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        retn    4
    }
}

}  // namespace NIpse
